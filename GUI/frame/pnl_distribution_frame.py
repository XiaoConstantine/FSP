from ttk import Frame

from ui.other_widget.status_bar import *
import ttk

import os
import Tkinter
import Image,ImageTk
from constants.constants import *
class PnlDistriutionFrame(ttk.Frame):
    '''
        classdocs
        '''
    NUM_BUCKETS = 6
    DIST_FILE_PATH = "data/pnl_distribution.dat"
    DIST_IMG_PATH = "data/pnl_distribution.png"
    PLOT_DIST_SCRIPT_PATH = "data/pnl_distribution.plt"
    BUCKET_FILE_PATH = "data/pnl_bucket.dat"
    BUCKET_IMG_PATH = "data/pnl_bucket.png"
    PLOT_BUCKET_SCRIPT_PATH = "data/pnl_bucket.plt"
    def __init__(self,master,data = None,**kw):
        '''
            Constructor
            '''
        ttk.Frame.__init__(self,master,**kw)
        self.status_bar = StatusBar(self)
        self.status_bar.grid(row = 1,column=0,sticky='news')
        self.set_data(data)
    
    def set_time(self,real_time,user_time,system_time):
        self.status_bar.setRealTime(real_time)
        self.status_bar.setUserTime(user_time)
        self.status_bar.setSystemTime(system_time)
    
    def set_data(self,data):
        if data==None:
            return
        table_data = data[TABLE_DATA]
        self.__write_data_to_file__(table_data)
        self.__set_up_image__()
        self.set_time(data[REAL_TIME], data[USER_TIME],
                      data[SYSTEM_TIME])

    def __write_data_to_file__(self,table_data):
        ##write distribution to file
        dist_file = open(PnlDistriutionFrame.DIST_FILE_PATH,"w")
        dist_file.write("index pnl\n")
        
        bucket_file = open(PnlDistriutionFrame.BUCKET_FILE_PATH,"w")
        bucket_file.write("index number\n")
        
        least_pnl = table_data[0]
        largest_pnl = table_data[-1]
        neg_bucket_width = abs(least_pnl)/PnlDistriutionFrame.NUM_BUCKETS
        pos_bucket_width = abs(largest_pnl)/PnlDistriutionFrame.NUM_BUCKETS
        lower_bound = least_pnl
        upper_bound = neg_bucket_width+least_pnl
        bucket_index = 1
        num_element = 0
        for index,pnl in enumerate(table_data):
            dist_file.write("%d %f\n" % (index,pnl))
            if pnl>=upper_bound:
                bucket_file.write("%d %d\n" % (bucket_index,num_element))
                num_element = 1
                lower_bound = upper_bound
                bucket_index+=1
                upper_bound =  lower_bound+pos_bucket_width if lower_bound>=0 else lower_bound+neg_bucket_width
            else:
                num_element+=1
        
        dist_file.close()
        bucket_file.close()
    
    def __set_up_image__(self):
        os.system("/usr/local/bin/gnuplot %s" % PnlDistriutionFrame.PLOT_DIST_SCRIPT_PATH)
        os.system("/usr/local/bin/gnuplot %s" % PnlDistriutionFrame.PLOT_BUCKET_SCRIPT_PATH)
        
        dim = Image.open(PnlDistriutionFrame.DIST_IMG_PATH)
        tkdim = ImageTk.PhotoImage(dim)
        label1 = Label(self,image=tkdim,relief=RIDGE)
        label1.image = tkdim
        label1.grid(row = 0,column=0,sticky='news')
        
        bim = Image.open(PnlDistriutionFrame.BUCKET_IMG_PATH)
        tkbim = ImageTk.PhotoImage(bim)
        label2 = Label(self,image=tkbim,relief=RIDGE)
        label2.image = tkbim
        label2.grid(row = 0,column=1,sticky='news')
