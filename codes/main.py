import struct
import numpy as np
# from scipy.sparse import csr_maxtrix
if __name__ == '__main__':
    filepath = "D:\\Tencent Files\\1910992493\\newData\\distance_info_2.txt"
    # filepath = "D:\\vs2017 c code\\Navigate\\Navigate\\input.txt"
    with open(filepath,'r') as file:
        lines = file.readlines()
    filepath2 = "D:\\vs2022 code\\navigate\\navigate\\newout.txt"
    # filepath2 = "D:\\vs2017 c code\\Navigate\\Navigate\\output.txt"
    with open(filepath2,'wb') as file2:
        for line in lines:
            #print(line)
            line_list = line.split()
            src = int(line_list[0])
            dst = int(line_list[1])
            dis = int(line_list[2])
            print(src,dst,dis)
            bytes = struct.pack('iii',src,dst,dis)
            file2.write(bytes)
    file2.close()
    file.close()