# coding=utf-8
import wx,sys,ctypes

class huffman(wx.Dialog):

    #initUI
    def __init__(self):
        wx.Dialog.__init__(self, None, -1, "huffman compress & decompress", size = (400, 400))
        self.SetToolTip(wx.ToolTip('This is a huffman made by lancer'))
        self.pathText = wx.TextCtrl(self, -1, "", pos = (10, 10), size = (375, 25))
        self.compressButton = wx.Button(self, -1, u"压缩", pos = (10, 45), size = (185, 40))
        self.Bind(wx.EVT_BUTTON, self.OnCompressClick, self.compressButton)
        self.decompressButton = wx.Button(self, -1, u"解压", pos = (200, 45), size = (185, 40))
        self.Bind(wx.EVT_BUTTON, self. OnDecompressClick, self.decompressButton)
        self.DisplayText = wx.TextCtrl(self, -1, "", pos = (10, 94), size = (373, 218), style = wx.TE_MULTILINE)
        self.printButton = wx.Button(self, -1, u"打印编码表", pos = (10, 322), size = (375, 40))
        self.Bind(wx.EVT_BUTTON, self.OnPrintClick, self.printButton)
        self.printButton.SetDefault()

    def OnPrintClick(self, event):
        file_object = open(self.pathText.GetValue()+'.log', 'rb')
        try:
            i = 0
            while i<1024:
                line = file_object.readline()
                self.DisplayText.AppendText(line.decode('gbk', 'replace'))
                i = i+1
        finally:
            file_object.close()
        return

    def OnCompressClick(self, event):
    	ll = ctypes.cdll.LoadLibrary
    	func = ll("./huffman.so")
    	in_filename = str(self.pathText.GetValue())
    	out_filename = in_filename+'.huf'
    	info = u"正在压缩……\n"
    	self.DisplayText.SetValue(info)
    	func.compress(in_filename,out_filename)
    	info += u"压缩完成！\n"
    	self.DisplayText.SetValue(info)
        return

    def OnDecompressClick(self, event):
        ll = ctypes.cdll.LoadLibrary
    	func = ll("./huffman.so")
    	in_filename = str(self.pathText.GetValue())
    	out_filename = in_filename[:-3]
    	info = u"正在解压……\n"
    	self.DisplayText.SetValue(info)
    	func.compress(in_filename,out_filename)
    	info += u"解压完成！\n"
    	self.DisplayText.SetValue(info)
        return

if __name__  ==  "__main__":
    app = wx.App()
    app.MainLoop()
    dialog = huffman()
    result = dialog.ShowModal()
    if result  ==  wx.ID_OK:
        print "OK"
    else:
        print "Cancel"
    dialog.Destroy()
