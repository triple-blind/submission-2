import subprocess, shlex
import xlwt
__author__ = 'Karthikeyan'

program = "C://Program Files (x86)/OpenOffice 4/program/autotype.exe"
i,j,k,l = 0,1,1,1
book = xlwt.Workbook()
sheet = book.add_sheet('test')
while i <= 1:
    while k < 4:
        subprocess.call([program, 'L', 'S'+str(i), 'P0', 'W0', 'G://Provenance/PerfTest/3wishes.txt', 'G://Provenance/PerfTest/log.txt'],shell=True)
        with open("G://Provenance/PerfTest/log.txt") as f:
            lines = f.readlines()
            last_row = lines[-1]
            time = last_row.split()
            print (time[1])
            if (k==1):
                sheet.write(l,1,'SuperSmall L S'+str(i)+' P0 W0')
            sheet.write(l,1+k,time[1])
        if(k==3):
            l += 1
        k += 1
    #book.save('G://Provenance/PerfTest/PerfOut.csv')
    while j <= 63:
        while k < 4:
            subprocess.call([program, 'L', 'S'+str(i), 'P1', 'W'+str(j), 'G://Provenance/PerfTest/3wishes.txt', 'G://Provenance/PerfTest/log.txt'],shell=True)
            with open("G://Provenance/PerfTest/log.txt") as f:
                lines = f.readlines()
                last_row = lines[-1]
                time = last_row.split()
                print (time[1])
                if (k==1):
                    sheet.write(l,1,'SuperSmall L S'+str(i)+' P1 W'+str(j))
                sheet.write(l,1+k,time[1])
            if (k==3):
                l += 1
            k += 1
        j += 1
        k=1
    i += 1
    book.save('G://Provenance/PerfTest/PerfOut.csv')