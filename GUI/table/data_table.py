import ttk
from Tkinter import *
from tkintertable.Tables import TableCanvas
from tkintertable.TableModels import TableModel

class DataTable(ttk.Frame):
    def __init__(self, master,data=None,columns=None,
                 upperColumnHeaders=None,**kw):
        ttk.Frame.__init__(self,master)
        self.tmodel = TableModel()
        if data!=None:
            self.tmodel.importDict(data,None,columns)
        self.table = self.create_table(self.tmodel,upperColumnHeaders,**kw)
        self.table.createTableFrame()
        return
    
    def set_data(self,data,columns):
        self.table.createfromDict(data,columns)
        self.table.createTableFrame()
    
    def create_table(self,tmodel,upperColumnHeaders,**kw):
        return TableCanvas(self, tmodel,upperColumnHeaders=upperColumnHeaders,**kw)
    
    def redraw(self):
        self.table.createTableFrame()