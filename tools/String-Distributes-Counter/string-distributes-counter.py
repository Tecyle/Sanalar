# -*- coding: utf-8 -*-
import os
import os.path
import codecs

folderList = ["utf-16-le", "utf-8", "gbk"]
expectList = u""
charCounter = {}
charCount = 0
totalCharCount = 0

def loadExpectList():
    global expectList
    expectList = codecs.open("expectList.txt", "r", "utf-8").read()

def isIgnored(ch):
    global expectList
    if ch >= u'\u4e00' and ch <= u'\u9fa5' and expectList.find(ch) == -1:
        return False
    else:
        return True

def coutingChars(text):
    global totalCharCount
    global charCounter
    global charCount
    for ch in text:
        totalCharCount += 1
        if isIgnored(ch):
            continue
        charCount += 1
        if charCounter.has_key(ch):
            charCounter[ch] += 1
        else:
            charCounter[ch] = 1

def generateReport():
    f = codecs.open("char_counter_report.txt", "w", "utf-8")
    text = u"数据库中共有字符数：%d\n" % (totalCharCount)
    f.write(text)
    text = u"共统计字符数：%d\n" % (charCount)
    f.write(text)
    text = u"共统计不重复的中文字符数：%d\n" % (len(charCounter))
    f.write(text)
    index = 1
    counter = 0
    sortedCharCounter = sorted(charCounter.items(), key = lambda e:e[1], reverse = True)
    for key in sortedCharCounter:
        counter += key[1]
        text = u"%d : %s = %d\t%.2f%%\n" % (index, key[0], key[1], counter * 100.0 / charCount)
        f.write(text)
        index += 1
    f.close()

loadExpectList()
for codePath in folderList:
    for parent, dirnames, filenames in os.walk("data\\txt\\" + codePath):
        for fileName in filenames:
            fileName = fileName.decode("gbk")
            parent = parent.decode("gbk")
            print u"正在统计： %s ...\n" % (os.path.join(parent, fileName))
            textContent = codecs.open(os.path.join(parent, fileName), "r", codePath, 'ignore').read().encode("utf-8")
            textContent = unicode(textContent, "utf-8")
            coutingChars(textContent)
    
generateReport()
