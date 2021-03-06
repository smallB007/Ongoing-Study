// 2015-05-13T17:57+08:00
// Load-balancing broker
// Demonstrates use of the CZMQ API and reactor style
//
// The client and worker tasks are identical from the previous example.
// .skip

#include "czmq.h"

#define NBR_CLIENTS 10
#define NBR_WORKERS 3
#define WORKER_READY   "READY"      //  Signals worker is ready

// Basic request-reply client using REQ socket
//
static void *
client_task(void *args)
{
	zctx_t *ctx = zctx_new();
	void *client = zsocket_new(ctx, ZMQ_REQ);

#if (defined (WIN32))
	zsocket_connect(client, "tcp://localhost:5672"); // frontend
#else
	zsocket_connect(client, "ipc://frontend.ipc");
#endif

	// Send request, get reply
	while (1) {
		zstr_send(client, "HELLO");
		char *reply = zstr_recv(client);
		if (!reply) {
			break;
		}
		printf("Client: %s\n", reply);
		free(reply);
		zclock_sleep(1);
	}

	zctx_destroy(&ctx);
	return NULL;
}

// Worker using REQ socket to do load-balancing
// 
static void *
worker_task(void *args)
{
	zctx_t *ctx = zctx_new();
	void *worker = zsocket_new(ctx, ZMQ_REQ);

#if (defined (WIN32))
	zsocket_connect(worker, "tcp://localhost:5673"); // backend
#else
	zsocket_connect(worker, "ipc://backend.ipc");
#endif

	// Tell broker we're ready for work
	zframe_t *frame = zframe_new(WORKER_READY, strlen(WORKER_READY));
	zframe_send(&frame, worker, 0);

	// Process messages as they arrive
	while (1) {
		zmsg_t *msg = zmsg_recv(worker);
		if (!msg) {
			break; // Interrupted
		}
		zframe_print(zmsg_last(msg), "Worker: ");
		zframe_reset(zmsg_last(msg), "OK", 2);
		zmsg_send(&msg, worker);
	}
	zctx_destroy(&ctx);
	return NULL;
}

// .until
// Our load-balancing structure, passed to reactor handlers
typedef struct {
	void *frontend; // Listen to clients
	void *backend; // Listen to workers
	zlist_t *workers; // List of ready workers
} lbbroker_t;

// .split reactor design
// In the reactor design, each time a message arrives on a socket, the
// reactor passes it to a handler function. We have two handlers; one 
// for the frontend, one for the backend:

// Handle input from client, on frontend
int s_handle_frontend(zloop_t *loop, zmq_pollitem_t *poller, void *arg)
{
	lbbroker_t *self = (lbbroker_t *)arg;
	zmsg_t *msg = zmsg_recv(self->frontend);
	if (msg) {
		zmsg_wrap(msg, (zframe_t *)zlist_pop(self->workers));
		zmsg_send(&msg, self->backend);

		// Cancel reader on frontend if we went from 1 to 0 workers
		if (zlist_size(self->workers) == 0) {
			zmq_pollitem_t poller = { self->frontend, 0, ZMQ_POLLIN };
			zloop_poller_end(loop, &poller);
		}
	}
	return 0;
}

// Handle input from worker, on backend
int s_handle_backend(zloop_t *loop, zmq_pollitem_t *poller, void *arg)
{
	// Use worker identity for load-balancing
	lbbroker_t *self = (lbbroker_t *)arg;
	zmsg_t *msg = zmsg_recv(self->backend);
	if (msg) {
		zframe_t *identity = zmsg_unwrap(msg);
		zlist_append(self->workers, identity);

		// Enable reader on frontend if we went from 0 to 1 workers
		if (zlist_size(self->workers) == 1) {
			zmq_pollitem_t poller = { self->frontend, 0, ZMQ_POLLIN };
			zloop_poller(loop, &poller, s_handle_frontend, self);
		}

		// Forward message to client if it's not a READY
		zframe_t *frame = zmsg_first(msg);
		if (memcmp(zframe_data(frame), WORKER_READY, strlen(WORKER_READY)) == 0) {
			zmsg_destroy(&msg);
		} else {
			zmsg_send(&msg, self->frontend);
		}
	}
	return 0;
}

// .split main task
// And the main task now sets up child tasks, then starts its reactor.
// If you press Ctrl-C, the reactor exits and the main task shuts down.
// Because the reactor is a CZMQ class, this example may not translate
// into all languages equally well.

int main(void)
{
	zctx_t *ctx = zctx_new();
	lbbroker_t *self = (lbbroker_t *)zmalloc(sizeof(lbbroker_t));
	self->frontend = zsocket_new(ctx, ZMQ_ROUTER);
	self->backend = zsocket_new(ctx, ZMQ_ROUTER);

#if (defined (WIN32))
	zsocket_bind(self->frontend, "tcp://*:5672"); // frontend
	zsocket_bind(self->backend, "tcp://*:5673"); // backend
#else
	zsocket_bind(self->frontend, "ipc://frontend.ipc");
	zsocket_bind(self->backend, "ipc://backend.ipc");
#endif

	int client_nbr;
	for (client_nbr = 0; client_nbr < NBR_CLIENTS; client_nbr++)
		zthread_new(client_task, NULL);
	int worker_nbr;
	for (worker_nbr = 0; worker_nbr < NBR_WORKERS; worker_nbr++)
		zthread_new(worker_task, NULL);

	// Queue of available workers
	self->workers = zlist_new();

	// Prepare reactor and fire it up
	zloop_t *reactor = zloop_new();
	zmq_pollitem_t poller = { self->backend, 0, ZMQ_POLLIN };
	zloop_poller(reactor, &poller, s_handle_backend, self);
	zloop_start(reactor);
	zloop_destroy(&reactor);

	// When we're done, clean up properly
	while (zlist_size(self->workers)) {
		zframe_t *frame = (zframe_t *)zlist_pop(self->workers);
		zframe_destroy(&frame);
	}
	zlist_destroy(&self->workers);
	zctx_destroy(&ctx);
	free(self);
	return 0;
}