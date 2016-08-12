﻿
using System.Drawing;
using System.Windows.Forms;

using DragableItem;

namespace DrawableItemTest
{
    public partial class DragableItemTest : Form
    {
        public DragableItemTest()
        {
            InitializeComponent();

            Pen linePen = new Pen(Color.Blue, 2.0f);
            Brush labelBackgroundBrush = new SolidBrush(Color.LightBlue);
            Font labelFont = new Font("Arial", 12.0f);
            Brush labelBrush = new SolidBrush(Color.Black);

            labeledLines = new DragableVerticalLabeledLine[2];
            string[] labels = { "Hello", "world" };
            for (int i = 0; i < labeledLines.Length; ++i)
            {
                labeledLines[i] = new DragableVerticalLabeledLine(this);
                labeledLines[i].Label = labels[i];
                labeledLines[i].LinePen = linePen;
                labeledLines[i].LabelEdgePen = linePen;
                labeledLines[i].LabelBackgroundBrush = labelBackgroundBrush;
                labeledLines[i].LabelFont = labelFont;
                labeledLines[i].LabelBrush = labelBrush;
                labeledLines[i].HoverCursor = Cursors.SizeWE;
            }
        }

        private DragableVerticalLabeledLine[] labeledLines;
        private Brush backgroundBrush = new SolidBrush(Color.SkyBlue);
        private Bitmap canvas;

        private void DrawAll(Graphics graphics)
        {
            Graphics g = Graphics.FromImage(canvas);
            g.FillRectangle(backgroundBrush, ClientRectangle);

            for (int i = 0; i < labeledLines.Length; ++i)
                labeledLines[i].Draw(g);

            graphics.DrawImage(canvas, ClientRectangle);
        }

        private void DragableItemTest_Layout(object sender, LayoutEventArgs e)
        {
            Point pt = new Point(0, 20);
            for (int i = 0; i < labeledLines.Length; ++i)
            {
                pt.X = ClientRectangle.Width / labeledLines.Length * i + 40;
                pt.Y = 20;
                labeledLines[i].Start = pt;
                pt.Y = ClientRectangle.Height - 20;
                labeledLines[i].End = pt;
            }

            canvas = new Bitmap(ClientRectangle.Width, ClientRectangle.Height);

            using (var g = CreateGraphics())
            {
                DrawAll(g);
            }
        }

        private void DragableItemTest_Paint(object sender, PaintEventArgs e)
        {
            DrawAll(e.Graphics);
        }

        private void DragableItemTest_MouseDown(object sender, MouseEventArgs e)
        {
            for (int i = 0; i < labeledLines.Length; ++i)
                labeledLines[i].OnMouseDown(sender, e);
        }

        private void DragableItemTest_MouseMove(object sender, MouseEventArgs e)
        {
            using (var g = CreateGraphics())
            {
                for (int i = 0; i < labeledLines.Length; ++i)
                {
                    if (labeledLines[i].OnMouseMove(sender, e))
                    {
                        DrawAll(g);
                    }
                }
            }
        }

        private void DragableItemTest_MouseUp(object sender, MouseEventArgs e)
        {
            for (int i = 0; i < labeledLines.Length; ++i)
                labeledLines[i].OnMouseUp(sender, e);
        }

    }
}