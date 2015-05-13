#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# 2014-10-27T10:43+08:00

def fetchPage(url, encoding = 'utf-8'):
    '''Fetch the source code of specified web page line by line.'''
    import urllib.request
    response = urllib.request.urlopen(url)

    if encoding:
        charset = response.headers.get_content_charset(failobj = encoding)
        for line in response:
            yield line.decode(charset)
    else:
        # Though HTTPResponse object is iterable, we CANNOT just return `response`
        # object here. Because `return` statement in a generator function has
        # special meanings. Instead, we should use `yield from`. See issue #1.
        #return response
        yield from response 

def main():
    '''Extract all Python tutorials urls from this page:
    http://www.bogotobogo.com/python/pytut.php
    '''
    import itertools
    import os
    import re

    line_generator = fetchPage('http://www.bogotobogo.com/python/pytut.php')

    # We can call builtin function iter in two ways(Please refer to the manual):
    # 1. iter(iterable)
    # 2. iter(callable, sentinel)
    # It seems that we can't pass a generator function to 'callable' in the 2nd
    # form, so I use itertools.takewhile here.
    for line in itertools.takewhile(
        lambda x: x.strip() != '<h1>List of Python Tutorials</h1>',
        line_generator):
        pass

    # Ignore all lines until we find the HTML list's start tag: <ul>
    for line in itertools.takewhile(lambda x: x.strip() != '<ul>',
                                    line_generator):
        pass

    # Extract all the urls in the HTML list until we reach the end tag: </ul>
    with open(os.path.join(os.environ['HOME'], 'List_of_Python_Tutorials.txt'),
              'w') as fp:
        url_pattern = re.compile(r'<a href="(.*)">')
        
        for line in itertools.takewhile(lambda x: x.strip() != '</ul>',
                                        line_generator):
            search_res = url_pattern.search(line)
            if search_res:
                print(search_res.group(1), file = fp)

if __name__ == '__main__':
    main()
