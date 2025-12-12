namespace SKKSetting
{
    partial class Form1
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージド リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing&&(components!=null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windowsフォームデザイナーで生成されたコード

        ///<summary>
        ///デザイナーサポートに必要なメソッドです。このメソッドの内容を
        ///コードエディターで変更しないでください。
        ///</summary>
        private void InitializeComponent()
        {
            this.label1=new System.Windows.Forms.Label();
            this.tabControl1=new System.Windows.Forms.TabControl();
            this.tabPage1=new System.Windows.Forms.TabPage();
            this.button3=new System.Windows.Forms.Button();
            this.button1=new System.Windows.Forms.Button();
            this.flowLayoutPanel1=new System.Windows.Forms.FlowLayoutPanel();
            this.tabPage2=new System.Windows.Forms.TabPage();
            this.collb=new System.Windows.Forms.ListBox();
            this.button4=new System.Windows.Forms.Button();
            this.button5=new System.Windows.Forms.Button();
            this.sankoutextcol=new System.Windows.Forms.Label();
            this.label4=new System.Windows.Forms.Label();
            this.button2=new System.Windows.Forms.Button();
            this.label3=new System.Windows.Forms.Label();
            this.button6=new System.Windows.Forms.Button();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.SuspendLayout();
            //
            //label1
            //
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("MS UI Gothic", 15F);
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(213, 30);
            this.label1.TabIndex = 0;
            this.label1.Text = "tettySKKの設定";
            //
            //tabControl1
            //
            this.tabControl1.Anchor=((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top|System.Windows.Forms.AnchorStyles.Bottom)
            |System.Windows.Forms.AnchorStyles.Left)
            |System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Location=new System.Drawing.Point(12, 42);
            this.tabControl1.Name="tabControl1";
            this.tabControl1.SelectedIndex=0;
            this.tabControl1.Size=new System.Drawing.Size(776, 409);
            this.tabControl1.TabIndex=1;
            //
            //tabPage1
            //
            this.tabPage1.Controls.Add(this.button3);
            this.tabPage1.Controls.Add(this.button1);
            this.tabPage1.Controls.Add(this.flowLayoutPanel1);
            this.tabPage1.Location=new System.Drawing.Point(4, 28);
            this.tabPage1.Name="tabPage1";
            this.tabPage1.Padding=new System.Windows.Forms.Padding(3);
            this.tabPage1.Size=new System.Drawing.Size(768, 377);
            this.tabPage1.TabIndex=0;
            this.tabPage1.Text="設定";
            this.tabPage1.UseVisualStyleBackColor=true;
            //
            //button3
            //
            this.button3.Anchor=((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom|System.Windows.Forms.AnchorStyles.Left)));
            this.button3.Location=new System.Drawing.Point(191, 337);
            this.button3.Name="button3";
            this.button3.Size=new System.Drawing.Size(100, 31);
            this.button3.TabIndex=3;
            this.button3.Text="初期化";
            this.button3.UseVisualStyleBackColor=true;
            this.button3.Click+=new System.EventHandler(this.button3_Click);
            //
            //button1
            //
            this.button1.Anchor=((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom|System.Windows.Forms.AnchorStyles.Left)));
            this.button1.Location=new System.Drawing.Point(9, 337);
            this.button1.Name="button1";
            this.button1.Size=new System.Drawing.Size(114, 31);
            this.button1.TabIndex=1;
            this.button1.Text="更新";
            this.button1.UseVisualStyleBackColor=true;
            this.button1.Click+=new System.EventHandler(this.button1_Click);
            //
            //flowLayoutPanel1
            //
            this.flowLayoutPanel1.Anchor=((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top|System.Windows.Forms.AnchorStyles.Bottom)
            |System.Windows.Forms.AnchorStyles.Left)
            |System.Windows.Forms.AnchorStyles.Right)));
            this.flowLayoutPanel1.AutoScroll=true;
            this.flowLayoutPanel1.BackColor=System.Drawing.Color.RosyBrown;
            this.flowLayoutPanel1.Location=new System.Drawing.Point(9, 6);
            this.flowLayoutPanel1.Name="flowLayoutPanel1";
            this.flowLayoutPanel1.Size=new System.Drawing.Size(756, 325);
            this.flowLayoutPanel1.TabIndex=0;
            //
            //tabPage2
            //
            this.tabPage2.Controls.Add(this.button6);
            this.tabPage2.Controls.Add(this.label3);
            this.tabPage2.Controls.Add(this.button2);
            this.tabPage2.Controls.Add(this.label4);
            this.tabPage2.Controls.Add(this.sankoutextcol);
            this.tabPage2.Controls.Add(this.button5);
            this.tabPage2.Controls.Add(this.button4);
            this.tabPage2.Controls.Add(this.collb);
            this.tabPage2.Location=new System.Drawing.Point(4, 28);
            this.tabPage2.Name="tabPage2";
            this.tabPage2.Padding=new System.Windows.Forms.Padding(3);
            this.tabPage2.Size=new System.Drawing.Size(768, 377);
            this.tabPage2.TabIndex=1;
            this.tabPage2.Text="色の設定";
            this.tabPage2.UseVisualStyleBackColor=true;
            //
            //collb
            //
            this.collb.Anchor=((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top|System.Windows.Forms.AnchorStyles.Bottom)
            |System.Windows.Forms.AnchorStyles.Left)));
            this.collb.Font=new System.Drawing.Font("MS UI Gothic", 13F);
            this.collb.FormattingEnabled=true;
            this.collb.ItemHeight=26;
            this.collb.Location=new System.Drawing.Point(16, 6);
            this.collb.Name="collb";
            this.collb.Size=new System.Drawing.Size(286, 342);
            this.collb.TabIndex=0;
            this.collb.SelectedIndexChanged+=new System.EventHandler(this.listBox1_SelectedIndexChanged);
            //
            //button4
            //
            this.button4.Anchor=((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom|System.Windows.Forms.AnchorStyles.Right)));
            this.button4.Location=new System.Drawing.Point(312, 320);
            this.button4.Name="button4";
            this.button4.Size=new System.Drawing.Size(106, 50);
            this.button4.TabIndex=1;
            this.button4.Text="更新";
            this.button4.UseVisualStyleBackColor=true;
            this.button4.Click+=new System.EventHandler(this.button4_Click);
            //
            //button5
            //
            this.button5.Anchor=((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom|System.Windows.Forms.AnchorStyles.Right)));
            this.button5.Location=new System.Drawing.Point(647, 324);
            this.button5.Name="button5";
            this.button5.Size=new System.Drawing.Size(105, 50);
            this.button5.TabIndex=2;
            this.button5.Text="初期化";
            this.button5.UseVisualStyleBackColor=true;
            this.button5.Click+=new System.EventHandler(this.button5_Click);
            //
            //sankoutextcol
            //
            this.sankoutextcol.Anchor=((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top|System.Windows.Forms.AnchorStyles.Bottom)
            |System.Windows.Forms.AnchorStyles.Left)
            |System.Windows.Forms.AnchorStyles.Right)));
            this.sankoutextcol.AutoSize=true;
            this.sankoutextcol.Font=new System.Drawing.Font("MSUIGothic", 12F);
            this.sankoutextcol.Location=new System.Drawing.Point(339, 47);
            this.sankoutextcol.Name="sankoutextcol";
            this.sankoutextcol.Size=new System.Drawing.Size(152, 96);
            this.sankoutextcol.TabIndex=3;
            this.sankoutextcol.Text="我輩は猫ナリ。\r\naccdeFGHIJK\r\n1234567890\r\n!\"#$%&@;:<>?\r\n";
            //
            //label4
            //
            this.label4.Anchor=((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top|System.Windows.Forms.AnchorStyles.Left)
            |System.Windows.Forms.AnchorStyles.Right)));
            this.label4.AutoSize=true;
            this.label4.Location=new System.Drawing.Point(504, 15);
            this.label4.Name="label4";
            this.label4.Size=new System.Drawing.Size(44, 18);
            this.label4.TabIndex=5;
            this.label4.Text="参考";
            //
            //button2
            //
            this.button2.Anchor=((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left|System.Windows.Forms.AnchorStyles.Right)));
            this.button2.BackColor=System.Drawing.Color.Red;
            this.button2.Location=new System.Drawing.Point(321, 247);
            this.button2.Margin=new System.Windows.Forms.Padding(0);
            this.button2.Name="button2";
            this.button2.Size=new System.Drawing.Size(431, 74);
            this.button2.TabIndex=6;
            this.button2.UseVisualStyleBackColor=false;
            this.button2.Click+=new System.EventHandler(this.button2_Click);
            //
            //label3
            //
            this.label3.AutoSize=true;
            this.label3.Location=new System.Drawing.Point(387, 215);
            this.label3.Name="label3";
            this.label3.Size=new System.Drawing.Size(0, 18);
            this.label3.TabIndex=7;
            //
            //button6
            //
            this.button6.Anchor=((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom|System.Windows.Forms.AnchorStyles.Right)));
            this.button6.Location=new System.Drawing.Point(435, 327);
            this.button6.Name="button6";
            this.button6.Size=new System.Drawing.Size(179, 37);
            this.button6.TabIndex=8;
            this.button6.Text="フォント設定";
            this.button6.UseVisualStyleBackColor=true;
            this.button6.Click+=new System.EventHandler(this.button6_Click);
            //
            //Form1
            //
            this.AutoScaleDimensions=new System.Drawing.SizeF(10F, 18F);
            this.AutoScaleMode=System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize=new System.Drawing.Size(800, 450);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.label1);
            this.Name="Form1";
            this.Text="tettySKK設定";
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.ListBox collb;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Label sankoutextcol;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button6;
        private System.Windows.Forms.Label label3;
    }
}

