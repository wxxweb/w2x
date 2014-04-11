#coding:utf-8
import os
import re
import sys
import string


ver_num = []


def usage():
    print "Modify program version number, using VC development and increasing its compiler version number."
    print "USAGE: python", sys.argv[0], "<rc> <major>.<minor>.<revision>"
    print "e.g.:  python", sys.argv[0], "resource.rc 1.0.1"

  
def checkArgv():
    #检测参数个数
    argc = len(sys.argv)
    if 3 != argc:
        print "The number of parameters does not match.\n"
        return False
    #检测资源文件是否存在
    if False == os.path.exists(sys.argv[1]):
        print "Not found file:", sys.argv[1], "\n"
        return
    #检测版本号格式是否正确
    global ver_num
    ver_num = sys.argv[2].split(".")
    if 3 != len(ver_num):
        print "Invalid version number.\n"
        return
    for i in range(0, 3):
        if False == ver_num[i].isdigit():
            print "Invalid version number.\n"
            return
    return True
            

def checkLine(file_, line_, name_, seq_):
    if -1 == line_.find(name_):
        file_.write(line_)
        return False
    
    print line_
    
    line_info = re.split("[ \t\n]", line_)
    index = -1
    count = len(line_info)
    for index in range(0, count):
        if -1 != line_info[index].find(seq_):
            break
    if -1 == index or index >= count:
        file_.write(line_)
        return False
    
    has_quote = False
    ver_str = line_info[index]
    if -1 != ver_str.find('"'):
        ver_str = string.strip(ver_str, '"')
        has_quote = True     
    ver_info = re.split("[" + seq_ + "]", ver_str)
    ver_info[3] = str(string.atoi(ver_info[3]) + 1)
    global ver_num
    ver_info[0] = ver_num[0]
    ver_info[1] = ver_num[1]
    ver_info[2] = ver_num[2]
    ver_str = seq_.join(ver_info)
    if has_quote:
        ver_str = '"' + ver_str + '"'
    line_info[index] = ver_str
    line_ = " ".join(line_info)
    file_.write(line_[:-1] + "\n")
    print line_
    return True
    
def scanResFile(file_path_):
    res_file = open(file_path_, "r")
    new_file = open(file_path_ + ".tmp", "w")
    is_scan_successed = False
    
    try:
        file_line = "x"
        while file_line:
            file_line = res_file.readline()
            if checkLine(new_file, file_line, "FILEVERSION", ","):
                break  
        while file_line:
            file_line = res_file.readline()
            if checkLine(new_file, file_line, "PRODUCTVERSION", ","):
                break
        while file_line:
            file_line = res_file.readline()
            if checkLine(new_file, file_line, '"FileVersion"', "."):
                break
        while file_line:
            file_line = res_file.readline()
            if checkLine(new_file, file_line, '"ProductVersion"', "."):
                break
        while file_line:
            file_line = res_file.readline()
            new_file.write(file_line)
            
        is_scan_successed = True
    finally:
        res_file.close()
        new_file.close()
        if not is_scan_successed:
            os.remove(file_path_ + ".tmp")
        
    if is_scan_successed:
        os.remove(file_path_)
        os.rename(file_path_ + ".tmp", file_path_)
    
    
if False == checkArgv():
    usage()
    exit()
scanResFile(sys.argv[1])
    
