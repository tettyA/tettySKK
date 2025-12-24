using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Xml.Linq;

namespace SKKSetting
{
    public partial class Form1 : Form
    {
        [DllImport("kernel32.dll")]
        private static extern int WritePrivateProfileString(
            string lpApplicationName,
            string lpKeyName,
            string lpstring,
            string lpFileName);

        [DllImport("kernel32.dll")]
        private static extern int GetPrivateProfileString(
                string lpApplicationName,
                string lpKeyName,
                string lpDefault,
                StringBuilder lpReturnedstring,
                int nSize,
                string lpFileName);

        [StructLayout(LayoutKind.Sequential)]
        public struct DEVMODE
        {
            private const int CCHDEVICENAME = 0x20;
            private const int CCHFORMNAME = 0x20;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 0x20)]
            public string dmDeviceName; // デバイス名
            public short dmSpecVersion;
            public short dmDriverVersion;
            public short dmSize; // DEVMODE構造体のサイズ
            public short dmDriverExtra;
            public int dmFields;
            public int dmPositionX; // ディスプレイ位置X
            public int dmPositionY; // ディスプレイ位置Y
            public ScreenOrientation dmDisplayOrientation;
            public int dmDisplayFixedOutput;
            public short dmColor;
            public short dmDuplex;
            public short dmYResolution;
            public short dmTTOption;
            public short dmCollate;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 0x20)]
            public string dmFormName;
            public short dmLogPixels;
            public int dmBitsPerPel;
            public int dmPelsWidth; // ディスプレイ解像度（横幅）
            public int dmPelsHeight; // ディスプレイ解像度（高さ）
            public int dmDisplayFlags;
            public int dmDisplayFrequency;
            public int dmICMMethod;
            public int dmICMIntent;
            public int dmMediaType;
            public int dmDitherType;
            public int dmReserved1;
            public int dmReserved2;
            public int dmPanningWidth;
            public int dmPanningHeight;
        }

        [DllImport("user32.dll", CharSet = CharSet.Ansi, SetLastError = true)]
        public static extern bool EnumDisplaySettingsA(string lpszDeviceName, int iModeNum, ref DEVMODE lpDevMode);



        public Form1()
        {
            InitializeComponent();
            InitializeOptionLowControl();
            initializeRomajiToKanaSettings();
        }
        struct OptionSetting
        {
            public string OptionText;
            public string IniText;
            public int DefaultValueIdx;
            public List<string> OptionValues;
            public List<string> IniValues;
            public OptionLowControl OptionControl;
        }
        List<OptionSetting> options =
        new List<OptionSetting>();

        List<int> whenStartSettingsOptValues;

        struct ColorOptionSetting
        {
            public string OptionText;
            public string IniText;
            public Color defaultVal;
            public Color val;
        }
        List<ColorOptionSetting> colorOptions =
     new List<ColorOptionSetting>();
        struct RomajiToKanaSetting
        {
            public string Romaji;
            public string HiraKana;
            public string KataKana;
        }

        List<RomajiToKanaSetting> romajiToKanaSettings = new List<RomajiToKanaSetting>();

        string IniFilePath = "tettySKKuserSettings.ini";
        string XmlFilePath = "tettySKKUserSettingsOpts.xml";
        string UserSettingsIni = "UserSetting";
        string ColorSettingsIni = "ColorSetting";

        string FontSettingFontNameIniKeyval;
        string FontSettingFontSizeIniKeyval;
        private string GetIniValue(string section, string key, string defaultValue)
        {
            StringBuilder retVal = new StringBuilder(255);
            GetPrivateProfileString(section, key, defaultValue, retVal, 255, IniFilePath);
            return retVal.ToString();
        }

        public void InitializeOptionLowControl()
        {
            options=new List<OptionSetting>();
            whenStartSettingsOptValues=new List<int>();
            XDocument xelement = XDocument.Load(XmlFilePath);


            //UserSettingsセクションのset要素を取得
            IEnumerable<XElement> optionElements = xelement.Root

            .Element("UserSettings")
            .Descendants("set");
            UserSettingsIni=xelement.Root
            .Element("UserSettings")
            .Attribute("iniSection").Value;
            foreach (XElement optionElement in optionElements)
            {
                OptionSetting optionSetting = new OptionSetting();
                optionSetting.OptionText=optionElement.Attribute("str").Value;
                optionSetting.IniText=optionElement.Attribute("iniName").Value;

                List<string> optvalues = new List<string>();
                List<string> inivalues = new List<string>();

                foreach (XElement valueElement in optionElement.Elements("opts"))
                {
                    int cnt = 0;
                    foreach (XElement v in valueElement.Elements("opt"))
                    {
                        optvalues.Add(v.Attribute("value").Value);
                        if (v.Attribute("default")!=null&&v.Attribute("default").Value=="true")
                        {
                            optionSetting.DefaultValueIdx=cnt;
                        }
                        inivalues.Add(v.Attribute("inival").Value);


                        cnt++;
                    }
                }
                optionSetting.OptionValues=optvalues;
                optionSetting.IniValues=inivalues;

                //iniから現在値を取得
                string iniVal =
                GetIniValue(UserSettingsIni, optionSetting.IniText, optionSetting.OptionValues[optionSetting.DefaultValueIdx]);

                whenStartSettingsOptValues.Add(
                optionSetting.IniValues.IndexOf(
                iniVal)
                );

                options.Add(optionSetting);

            }


            for (int i = 0; i < options.Count; i++)
            {
                OptionSetting opSetting = options[i];
                OptionLowControl op = new OptionLowControl();
                op.SetOption(opSetting.OptionText, opSetting.OptionValues);
                op.SelectOption(whenStartSettingsOptValues[i]);

                opSetting.OptionControl=op;

                options[i]=opSetting;

                flowLayoutPanel1.Controls.Add(op);
            }

            {
                //Color設定など
                IEnumerable<XElement> colorOptionElements2 = xelement.Root
                .Element("ColorSettings")
                .Descendants("set");
                ColorSettingsIni=xelement.Root.Element("ColorSettings").Attribute("iniSection").Value;

                colorOptions=new List<ColorOptionSetting>();

                foreach (XElement el in colorOptionElements2)
                {
                    if (el.Attribute("valtype").Value=="RGB")
                    {
                        ColorOptionSetting coloos = new ColorOptionSetting();
                        coloos.defaultVal=Color.FromArgb(
                        int.Parse(el.Element("defval").Attribute("r").Value),
                        int.Parse(el.Element("defval").Attribute("g").Value),
                        int.Parse(el.Element("defval").Attribute("b").Value)
                        );
                        coloos.OptionText=el.Attribute("str").Value;
                        coloos.IniText=el.Attribute("iniName").Value;

                        string valstr = GetIniValue(ColorSettingsIni, coloos.IniText, $"{TranslateR_G_B_toRGB(coloos.defaultVal)}");

                        coloos.val=TranslateRGBToR_G_B(int.Parse(valstr));

                        colorOptions.Add(coloos);

                        collb.Items.Add(coloos.OptionText);
                    }
                    else if (el.Attribute("valtype").Value=="font")
                    {
                        Font tmpfont = sankoutextcol.Font;
                        switch (el.Attribute("subvaltype").Value)
                        {
                            case "csize":
                                FontSettingFontSizeIniKeyval=el.Attribute("iniName").Value;

                                tmpfont=new Font(tmpfont.FontFamily, TranslatecHeightToFontSize(int.Parse(GetIniValue(ColorSettingsIni, FontSettingFontSizeIniKeyval, el.Element("defval").Attribute("size").Value.ToString())), tmpfont.FontFamily.Name));
                                break;
                            case "fname":
                                FontSettingFontNameIniKeyval=el.Attribute("iniName").Value;
                                tmpfont=new Font(GetIniValue(ColorSettingsIni, FontSettingFontNameIniKeyval, el.Element("defval").Attribute("name").Value.ToString()), tmpfont.Size);
                                break;
                            default:
                                break;
                        }

                        sankoutextcol.Font=tmpfont;
                    }
                }
            }
        }

        private float TranslatecHeightToFontSize(int cHeight, string fontFamilyName)
        {
            float dpiY;
            using (Graphics g = Graphics.FromHwnd(IntPtr.Zero))
            {
                dpiY=g.DpiY;
            }

            float designEmHeight;
            float designLineSpacing;

            using (FontFamily ff = new FontFamily(fontFamilyName))
            {
                designEmHeight=ff.GetEmHeight(FontStyle.Regular);
                designLineSpacing=ff.GetLineSpacing(FontStyle.Regular);
            }

            float cellHeight = cHeight*(72.0f/dpiY);

            float scale;
            {
                Screen screen = Screen.FromPoint(Cursor.Position);
                DEVMODE dm = new DEVMODE();
                dm.dmSize=(short)Marshal.SizeOf(typeof(DEVMODE));
                int ENUM_CURRENT_SETTINGS = -1;
                EnumDisplaySettingsA(screen.DeviceName, ENUM_CURRENT_SETTINGS, ref dm);
                scale=(float)(dm.dmPelsWidth)/screen.Bounds.Width;
            }

            return cellHeight*(designEmHeight/designLineSpacing)/scale;
        }

        private int TranslateFontSizeTocHeight(float FontSize, string fontFamilyName)
        {
            float dpiY;
            using (Graphics g = Graphics.FromHwnd(IntPtr.Zero))
            {
                dpiY=g.DpiY;
            }
            float designEmHeight;
            float designLineSpacing;

            using (FontFamily ff = new FontFamily(fontFamilyName))
            {
                designEmHeight=ff.GetEmHeight(FontStyle.Regular);
                designLineSpacing=ff.GetLineSpacing(FontStyle.Regular);
            }

            float emHeightPixels = (int)(FontSize*dpiY/72.0);

            float scale;
            {
                Screen screen = Screen.FromPoint(Cursor.Position);
                DEVMODE dm = new DEVMODE();
                dm.dmSize=(short)Marshal.SizeOf(typeof(DEVMODE));
                int ENUM_CURRENT_SETTINGS = -1;
                EnumDisplaySettingsA(screen.DeviceName, ENUM_CURRENT_SETTINGS, ref dm);
                scale=(float)(dm.dmPelsWidth)/screen.Bounds.Width;
            }

            float cellHeightPixels = emHeightPixels*(designLineSpacing/designEmHeight)*scale;
            return (int)cellHeightPixels;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //設定の更新
            foreach (var opt in options)
            {
                int selectedIdx = opt.OptionControl.GetSelectedOption();
                if (selectedIdx<0)
                {
                    selectedIdx=opt.DefaultValueIdx;
                }
                WritePrivateProfileString(UserSettingsIni, opt.IniText, opt.IniValues[selectedIdx], IniFilePath);
            }

        }

        private void button3_Click(object sender, EventArgs e)
        {
            foreach (var opt in options)
            {
                opt.OptionControl.SelectOption(opt.DefaultValueIdx);
            }
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            ChangeSankouColor();
        }

        void ChangeSankouColor()
        {
            sankoutextcol.ForeColor=
            colorOptions[collb.SelectedIndex].val;
            button2.BackColor=colorOptions[collb.SelectedIndex].val;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (collb.SelectedIndex<0) return;

            ColorDialog coldiag = new ColorDialog();
            coldiag.Color=colorOptions[collb.SelectedIndex].val;
            if (coldiag.ShowDialog()==DialogResult.OK)
            {
                ColorOptionSetting tmpcol = colorOptions[collb.SelectedIndex];
                tmpcol.val=coldiag.Color;
                colorOptions[collb.SelectedIndex]=tmpcol;

                ChangeSankouColor();
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            for (int i = 0; i<colorOptions.Count; i++)
            {
                ColorOptionSetting tmpcos = colorOptions[i];
                tmpcos.val=tmpcos.defaultVal;

                colorOptions[i]=tmpcos;
            }

            ChangeSankouColor();
        }

        private void button6_Click(object sender, EventArgs e)
        {
            FontDialog fd = new FontDialog();
            fd.Font=sankoutextcol.Font;
            fd.Color=sankoutextcol.ForeColor;
            fd.FontMustExist=true;
            fd.ShowEffects=false;

            if (fd.ShowDialog()!=DialogResult.Cancel)
            {
                sankoutextcol.Font=fd.Font;
            }

        }
        int TranslateR_G_B_toRGB(Color col)
        {
            return ColorTranslator.ToWin32(col);
        }
        Color TranslateRGBToR_G_B(int RGB)
        {
            return ColorTranslator.FromWin32(RGB);
        }
        private void button4_Click(object sender, EventArgs e)
        {
            //設定の更新
            foreach (var opt in colorOptions)
            {
                WritePrivateProfileString(ColorSettingsIni, opt.IniText, $"{TranslateR_G_B_toRGB(opt.val)}", IniFilePath);
            }

            WritePrivateProfileString(ColorSettingsIni, FontSettingFontNameIniKeyval, sankoutextcol.Font.FontFamily.Name, IniFilePath);
            WritePrivateProfileString(ColorSettingsIni, FontSettingFontSizeIniKeyval, TranslateFontSizeTocHeight(sankoutextcol.Font.Size, sankoutextcol.Font.FontFamily.Name).ToString(), IniFilePath);
        }

        public void initializeRomajiToKanaSettings()
        {
            romajiToKanaSettings=new List<RomajiToKanaSetting>();
//TODO: ここに本当の値を直に載せる。

            using (StreamReader sr = new StreamReader(@"hogehoge", Encoding.GetEncoding("shift_jis")))
            {
                while (!sr.EndOfStream)
                {
                    string line = sr.ReadLine();
                    line=line.Replace(" ", "").Replace("\t", "");
                    string[] parts = line.Split(',');
                    if (parts.Length>=3)
                    {
                        RomajiToKanaSetting setting = new RomajiToKanaSetting();
                        setting.Romaji=parts[0];
                        setting.HiraKana=parts[1];
                        setting.KataKana=parts[2];
                        romajiToKanaSettings.Add(setting);
                        ListViewItem lvi = new ListViewItem(new string[] { parts[0], parts[1], parts[2] });
                        listViewhiraganatrans.Items.Add(lvi);
                    }
                }
            }
        }

        private void listViewhiraganatrans_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listViewhiraganatrans.SelectedItems.Count==0) return;
            textBoxromaji.Text=listViewhiraganatrans.SelectedItems[0].SubItems[0].Text;
            textBoxhiragana.Text=listViewhiraganatrans.SelectedItems[0].SubItems[1].Text;
            textBoxkatakana.Text=listViewhiraganatrans.SelectedItems[0].SubItems[2].Text;
        }

        private void button7_Click(object sender, EventArgs e)
        {
            if (listViewhiraganatrans.SelectedItems.Count==0) return;
            listViewhiraganatrans.SelectedItems[0].SubItems[0].Text=textBoxromaji.Text;
            listViewhiraganatrans.SelectedItems[0].SubItems[1].Text=textBoxhiragana.Text;
            listViewhiraganatrans.SelectedItems[0].SubItems[2].Text=textBoxkatakana.Text;
        }

        private void button9_Click(object sender, EventArgs e)
        {
            if (listViewhiraganatrans.SelectedItems.Count==0) return;
            listViewhiraganatrans.Items.Add(
                new ListViewItem(
                    new string[]
                    {
                        textBoxromaji.Text,
                        textBoxhiragana.Text,
                        textBoxkatakana.Text
                    }
                )
            );
        }

        private void button10_Click(object sender, EventArgs e)
        {
            if (listViewhiraganatrans.SelectedItems.Count==0) return;
            listViewhiraganatrans.Items.Remove(listViewhiraganatrans.SelectedItems[0]);
        }
    }
}
