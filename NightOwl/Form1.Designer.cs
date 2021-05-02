
namespace NightOwl
{
    partial class NightOwlForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.MenuStrip = new System.Windows.Forms.MenuStrip();
            this.ToolStripMenuIItemEdit = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItemFileDropDownMenuItemExit = new System.Windows.Forms.ToolStripMenuItem();
            this.FlowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.ShaderDirectoryPathBrowseButton = new System.Windows.Forms.Button();
            this.ShaderDirectoryWatcherRunButton = new System.Windows.Forms.Button();
            this.PathGroupBox = new System.Windows.Forms.GroupBox();
            this.VulkanDirectoryRichTextBox = new System.Windows.Forms.RichTextBox();
            this.ShaderDirectoryWatcherPathRichTextBox = new System.Windows.Forms.RichTextBox();
            this.ShaderDirectoryWatcherRichTextBox = new System.Windows.Forms.RichTextBox();
            this.FolderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.MenuStrip.SuspendLayout();
            this.FlowLayoutPanel1.SuspendLayout();
            this.PathGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // MenuStrip
            // 
            this.MenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuIItemEdit});
            this.MenuStrip.Location = new System.Drawing.Point(0, 0);
            this.MenuStrip.Name = "MenuStrip";
            this.MenuStrip.Size = new System.Drawing.Size(1904, 24);
            this.MenuStrip.TabIndex = 0;
            this.MenuStrip.Text = "menuStrip1";
            // 
            // ToolStripMenuIItemEdit
            // 
            this.ToolStripMenuIItemEdit.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItemFileDropDownMenuItemExit});
            this.ToolStripMenuIItemEdit.Name = "ToolStripMenuIItemEdit";
            this.ToolStripMenuIItemEdit.Size = new System.Drawing.Size(37, 20);
            this.ToolStripMenuIItemEdit.Text = "File";
            // 
            // ToolStripMenuItemFileDropDownMenuItemExit
            // 
            this.ToolStripMenuItemFileDropDownMenuItemExit.Name = "ToolStripMenuItemFileDropDownMenuItemExit";
            this.ToolStripMenuItemFileDropDownMenuItemExit.Size = new System.Drawing.Size(93, 22);
            this.ToolStripMenuItemFileDropDownMenuItemExit.Text = "Exit";
            this.ToolStripMenuItemFileDropDownMenuItemExit.Click += new System.EventHandler(this.toolStripMenuItemFileDropDownMenuItemExit_Click);
            // 
            // FlowLayoutPanel1
            // 
            this.FlowLayoutPanel1.Controls.Add(this.ShaderDirectoryPathBrowseButton);
            this.FlowLayoutPanel1.Controls.Add(this.ShaderDirectoryWatcherRunButton);
            this.FlowLayoutPanel1.Controls.Add(this.PathGroupBox);
            this.FlowLayoutPanel1.Controls.Add(this.ShaderDirectoryWatcherRichTextBox);
            this.FlowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.FlowLayoutPanel1.Location = new System.Drawing.Point(0, 24);
            this.FlowLayoutPanel1.Name = "FlowLayoutPanel1";
            this.FlowLayoutPanel1.Size = new System.Drawing.Size(1904, 1017);
            this.FlowLayoutPanel1.TabIndex = 1;
            // 
            // ShaderDirectoryPathBrowseButton
            // 
            this.ShaderDirectoryPathBrowseButton.Location = new System.Drawing.Point(128, 32);
            this.ShaderDirectoryPathBrowseButton.Margin = new System.Windows.Forms.Padding(128, 32, 0, 0);
            this.ShaderDirectoryPathBrowseButton.Name = "ShaderDirectoryPathBrowseButton";
            this.ShaderDirectoryPathBrowseButton.Size = new System.Drawing.Size(128, 128);
            this.ShaderDirectoryPathBrowseButton.TabIndex = 0;
            this.ShaderDirectoryPathBrowseButton.Text = "Browse";
            this.ShaderDirectoryPathBrowseButton.UseVisualStyleBackColor = true;
            this.ShaderDirectoryPathBrowseButton.Click += new System.EventHandler(this.BrowseButton_Click);
            // 
            // ShaderDirectoryWatcherRunButton
            // 
            this.ShaderDirectoryWatcherRunButton.Location = new System.Drawing.Point(272, 32);
            this.ShaderDirectoryWatcherRunButton.Margin = new System.Windows.Forms.Padding(16, 32, 16, 32);
            this.ShaderDirectoryWatcherRunButton.Name = "ShaderDirectoryWatcherRunButton";
            this.ShaderDirectoryWatcherRunButton.Size = new System.Drawing.Size(128, 128);
            this.ShaderDirectoryWatcherRunButton.TabIndex = 2;
            this.ShaderDirectoryWatcherRunButton.Text = "Watch";
            this.ShaderDirectoryWatcherRunButton.UseVisualStyleBackColor = true;
            this.ShaderDirectoryWatcherRunButton.Click += new System.EventHandler(this.ShaderDirectoryWatcherRunButton_Click);
            // 
            // PathGroupBox
            // 
            this.PathGroupBox.Controls.Add(this.VulkanDirectoryRichTextBox);
            this.PathGroupBox.Controls.Add(this.ShaderDirectoryWatcherPathRichTextBox);
            this.PathGroupBox.Location = new System.Drawing.Point(419, 3);
            this.PathGroupBox.Name = "PathGroupBox";
            this.PathGroupBox.Size = new System.Drawing.Size(1473, 186);
            this.PathGroupBox.TabIndex = 4;
            this.PathGroupBox.TabStop = false;
            this.PathGroupBox.Text = "Path";
            // 
            // VulkanDirectoryRichTextBox
            // 
            this.VulkanDirectoryRichTextBox.BackColor = System.Drawing.SystemColors.Window;
            this.VulkanDirectoryRichTextBox.Location = new System.Drawing.Point(35, 83);
            this.VulkanDirectoryRichTextBox.Margin = new System.Windows.Forms.Padding(32, 32, 0, 0);
            this.VulkanDirectoryRichTextBox.Name = "VulkanDirectoryRichTextBox";
            this.VulkanDirectoryRichTextBox.Size = new System.Drawing.Size(512, 33);
            this.VulkanDirectoryRichTextBox.TabIndex = 2;
            this.VulkanDirectoryRichTextBox.Text = "C:/VulkanSDK/1.2.148.1/Bin32/glslc.exe";
            // 
            // ShaderDirectoryWatcherPathRichTextBox
            // 
            this.ShaderDirectoryWatcherPathRichTextBox.BackColor = System.Drawing.SystemColors.Window;
            this.ShaderDirectoryWatcherPathRichTextBox.Location = new System.Drawing.Point(35, 29);
            this.ShaderDirectoryWatcherPathRichTextBox.Margin = new System.Windows.Forms.Padding(32, 32, 0, 0);
            this.ShaderDirectoryWatcherPathRichTextBox.Name = "ShaderDirectoryWatcherPathRichTextBox";
            this.ShaderDirectoryWatcherPathRichTextBox.Size = new System.Drawing.Size(512, 33);
            this.ShaderDirectoryWatcherPathRichTextBox.TabIndex = 1;
            this.ShaderDirectoryWatcherPathRichTextBox.Text = "C:/Users/Ray/Desktop/Code/TenetEngine/Campfire/Shaders";
            // 
            // ShaderDirectoryWatcherRichTextBox
            // 
            this.ShaderDirectoryWatcherRichTextBox.Location = new System.Drawing.Point(3, 195);
            this.ShaderDirectoryWatcherRichTextBox.Name = "ShaderDirectoryWatcherRichTextBox";
            this.ShaderDirectoryWatcherRichTextBox.Size = new System.Drawing.Size(1904, 854);
            this.ShaderDirectoryWatcherRichTextBox.TabIndex = 3;
            this.ShaderDirectoryWatcherRichTextBox.Text = "";
            // 
            // NightOwlForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1904, 1041);
            this.Controls.Add(this.FlowLayoutPanel1);
            this.Controls.Add(this.MenuStrip);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Name = "NightOwlForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "NightOwl";
            this.MenuStrip.ResumeLayout(false);
            this.MenuStrip.PerformLayout();
            this.FlowLayoutPanel1.ResumeLayout(false);
            this.PathGroupBox.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip MenuStrip;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuIItemEdit;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItemFileDropDownMenuItemExit;
        private System.Windows.Forms.FlowLayoutPanel FlowLayoutPanel1;
        private System.Windows.Forms.Button ShaderDirectoryPathBrowseButton;
        private System.Windows.Forms.Button ShaderDirectoryWatcherRunButton;
        private System.Windows.Forms.RichTextBox ShaderDirectoryWatcherPathRichTextBox;
        private System.Windows.Forms.FolderBrowserDialog FolderBrowserDialog;
        private System.Windows.Forms.RichTextBox ShaderDirectoryWatcherRichTextBox;
        private System.Windows.Forms.GroupBox PathGroupBox;
        private System.Windows.Forms.RichTextBox VulkanDirectoryRichTextBox;
        private System.IO.FileSystemWatcher FileSystemWatcher;
    }
}

