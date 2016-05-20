#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Author: 方直
# @Date:   2015-12-30 23:11:32
# @Last Modified by:   方直
# @Last Modified time: 2015-12-30 23:16:33

# coding=utf-8
import wx,sys,ctypes

class huffman(wx.Dialog):

    #initUI
    def __init__(self):
        wx.Dialog.__init__(self, None, -1, "huffman compress & decompress made by lancer", size = (400, 400))
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
        return

    def OnCompressClick(self, event):
        ll = ctypes.cdll.LoadLibrary
        func = ll("./huffman.so")
        in_filename = str(self.pathText.GetValue())
        out_filename = ''
        self.DisplayText.SetValue(u"正在压缩……\n")
        func.compress(in_filename,out_filename)
        return

    def OnDecompressClick(self, event):
        ll = ctypes.cdll.LoadLibrary
        func = ll("./huffman.so")
        in_filename = str(self.pathText.GetValue())
        out_filename = ''
        self.DisplayText.SetValue(u"正在解压……\n")
        func.decompress(in_filename,out_filename)
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
