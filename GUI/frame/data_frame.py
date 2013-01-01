import ttk
from Tkinter import *
class MyDataFrame(ttk.Frame):
    '''
        classdocs
        '''
    
    
    def __init__(self,master,**kw):
        '''
            Constructor
            '''
        ttk.Frame.__init__(self, master,**kw)
    
    def transform_data(self,data):
        if (data == None or len(data)<1 or len(data[0]) != len(self.COLUMNS)):
            return None
        result = {}
        index =1
        for entry in data:
            dict = {}
            i = 0
            for attr in entry:
                dict[self.COLUMNS[i]]=attr
                i+=1
            result[index] = dict
            index+=1
        return result