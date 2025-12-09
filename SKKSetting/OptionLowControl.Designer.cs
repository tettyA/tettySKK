using System.Collections.Generic;
using System.Data.OleDb;
using System.Windows.Forms;

namespace SKKSetting
{
    partial class OptionLowControl
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
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region コンポーネント デザイナーで生成されたコード

        /// <summary> 
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を 
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.SetOptText = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // SetOptText
            // 
            this.SetOptText.AutoSize = true;
            this.SetOptText.Font = new System.Drawing.Font("MS UI Gothic", 12F);
            this.SetOptText.Location = new System.Drawing.Point(3, 12);
            this.SetOptText.Name = "SetOptText";
            this.SetOptText.Size = new System.Drawing.Size(174, 24);
            this.SetOptText.TabIndex = 0;
            this.SetOptText.Text = "設定項目の説明";
            // 
            // OptionLowControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.SetOptText);
            this.Name = "OptionLowControl";
            this.Size = new System.Drawing.Size(1742, 59);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        List<RadioButton> rajioBtns;
        public void SetOption(string text,List<string> rajio_opts)
        {
            SetOptText.Text = text;

            rajioBtns = new List<RadioButton>();
            int addx= 200;
            for (int i = 0; i < rajio_opts.Count; i++)
            {
                rajioBtns.Add(new RadioButton());
                rajioBtns[i].AutoSize = true;
                rajioBtns[i].Font = new System.Drawing.Font("MS UI Gothic", 10F);
                rajioBtns[i].Location = new System.Drawing.Point(addx, SetOptText.Location.Y);
                rajioBtns[i].Name = "rajioBtn" + i.ToString();
                rajioBtns[i].Visible = true;
                rajioBtns[i].Text= rajio_opts[i];

                addx += rajioBtns[i].Width;
            }

            this.Controls.AddRange(rajioBtns.ToArray());
            this.ResumeLayout(false);
        }

        public int GetSelectedOption()
        {
            for (int i = 0; i < rajioBtns.Count; i++)
            {
                if (rajioBtns[i].Checked)
                {
                    return i;
                }
            }
            return -1;
        }

        public void SelectOption(int idx)
        {
            if(idx>=0 && idx<rajioBtns.Count)
            {
                rajioBtns[idx].Checked = true;
            }
        }
        private System.Windows.Forms.Label SetOptText;
    }
}
