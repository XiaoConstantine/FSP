from ttk import Frame
from Tkinter import *
from ui.other_widget.status_bar import *
from ui.table.data_table import *
from ui.frame.data_frame import MyDataFrame
from tkintertable.TableFormula import Formula
import tkMessageBox
from constants.constants import *

m_count = 0
class YieldCurveDataTableCanvas(TableCanvas):
    def __init__(self, parent=None, model=None, newdict=None, width=None, height=None, **kwargs):
        TableCanvas.__init__(self, parent, model, newdict, width, height, **kwargs)
        self.bump_curve = kwargs['bump_curve']
    
    def draw_cellentry(self, row, col, text=None):
        """When the user single/double clicks on a text/number cell, bring up entry window"""
        if self.editable == False:
            return
        absrow = self.get_AbsoluteRow(row)
        h=self.rowheight
        model=self.getModel()
        cellvalue = self.model.getCellRecord(absrow, col)
        if Formula.isFormula(cellvalue):
            return
        else:
            text = self.model.getValueAt(absrow, col)
        x1,y1,x2,y2 = self.getCellCoords(row,col)
        w=x2-x1
        #Draw an entry window
        txtvar = StringVar()
        txtvar.set(text)
        def callback(e):
            value = txtvar.get()
            if value == '=':
                
                #do a dialog that gets the formula into a text area
                #then they can click on the cells they want
                #when done the user presses ok and its entered into the cell
                self.cellentry.destroy()
                #its all done here..
                self.formula_Dialog(row, col)
                return
            
            coltype = self.model.getColumnType(col)
            if coltype == 'number':
                sta = self.check_data_entry(e)
                if sta == 1:
                    model.setValueAt(value,absrow,col)
            elif coltype == 'text':
                model.setValueAt(value,absrow,col)
            
            color = self.model.getColorAt(absrow,col,'fg')
            
            if e.keysym=='Return':
                global m_count
                self.delete('entry')
                if col == 8:
                    if m_count == 1:
                        m_count = 0
                        return
                    m_count+=1
                    maturity = self.model.getRecordAtRow(row)[YieldCurveFrame.COLUMNS[0]]
                    yield_rate_str = self.model.getRecordAtRow(row)[YieldCurveFrame.COLUMNS[col]]
                    if (not yield_rate_str) or (not yield_rate_str.strip()):
                        return
                    spread = 0
                    try:
                        
                        spread_f = float(yield_rate_str)
                        spread_i = int(yield_rate_str)
                        if spread_f!=spread_i:
                            raise BaseException
                        spread =  spread_i
                    except BaseException:
                        tkMessageBox.showwarning("Illegal number format", "Please input a integer number here!")
                        value = ""
                        self.model.setValueAt("", row, col)
                        self.draw_Text(row, col, value, color)
                        return
                    self.bump_curve(maturity,spread)
            #self.draw_rect(row, col)
            #self.gotonextCell(e)
            self.draw_Text(row, col, value, color)
            return

        self.cellentry=Entry(self.parentframe,width=20,
                             textvariable=txtvar,
                             bg=self.entrybackgr,
                             relief=FLAT,
                             takefocus=1,
                             font=self.thefont)
        self.cellentry.icursor(END)
        self.cellentry.bind('<Return>', callback)
        self.cellentry.bind('<KeyRelease>', callback)
        self.cellentry.focus_set()
        self.entrywin=self.create_window(x1+self.inset,y1+self.inset,
                                         width=w-self.inset*2,height=h-self.inset*2,
                                         window=self.cellentry,anchor='nw',
                                         tag='entry')
        
        return
    
    def find_row_by_index(self,index):
        return self.getModel().getRecordAtRow(index)


class YieldCurveDataTable(DataTable):
    def __init__(self, master,data=None,columns=None,
                 upperColumnHeaders=None,**kw):
        DataTable.__init__(self, master,data,columns,
                           upperColumnHeaders,**kw)
    
    def create_table(self,tmodel,upperColumnHeaders,**kw):
        return YieldCurveDataTableCanvas(self, tmodel,upperColumnHeaders=upperColumnHeaders,**kw)


class YieldCurveFrame(MyDataFrame):
    '''
        classdocs
        '''
    
    COLUMNS = ["Maturity","Risk","Mkt Value","2yr hedge","Shifted risk","Shifted position","Shifted 2 yr hedge","Yield Curve","Bump Curve"]
    YEARS = ["2YR","5YR","10YR","30YR"]
    def __init__(self,master,data=None,service = None,**kw):
        '''
            Constructor
            '''
        MyDataFrame.__init__(self, master,**kw)
        self.service = service
        self.pack(fill=BOTH)
        table_data = self.transform_data(self.__transform_data__(data[TABLE_DATA]))
        self.yield_curve_table = YieldCurveDataTable(self,table_data,
                                                     YieldCurveFrame.COLUMNS,None,width=1080,height=500,bump_curve = self.bump_curve)
        
        self.yield_curve_table.grid(row = 0,column = 0,sticky='news')
        
        self.button_panel = self.create_button_frame()
        self.button_panel.grid(row = 1,column = 0,sticky='news')
        
        self.status_bar = StatusBar(self)
        self.status_bar.grid(row = 2,column = 0,sticky='news')
        if data != None:
            self.set_data(data)
    
    def create_button_frame(self):
        tframe = ttk.Frame(self)
        
        up_button = ttk.Button(tframe,text = "shirt 50% up",command = self.shift_50p_up)
        up_button.pack(side="left")
        
        down_button = ttk.Button(tframe,text = "shirt 50% down",command = self.shift_50p_down)
        down_button.pack(side="left")
        
        original_button = ttk.Button(tframe,text = "original curve",command = self.back_to_original)
        original_button.pack(side="left")
        return tframe
    
    def shift_50p_up(self):
        shifted_data = self.service.shift_50_up()
        #        for maturity,data in shifted_data[TABLE_DATA].iteritems():
        #            row = self.yield_curve_table.table.find_row_by_index(self.maturity_map[maturity])
        #            row[YieldCurveFrame.COLUMNS[-4]] = data[YieldCurveFrame.COLUMNS[-4]]
        #            row[YieldCurveFrame.COLUMNS[-3]] = data[YieldCurveFrame.COLUMNS[-3]]
        #        self.yield_curve_table.redraw()
        #        self.set_time(shifted_data[REAL_TIME], shifted_data[USER_TIME], shifted_data[SYSTEM_TIME])
        self.set_data(shifted_data)
    
    
    def shift_50p_down(self):
        shifted_data = self.service.shift_50_down()
        self.set_data(shifted_data)
    #        for maturity,data in shifted_data[TABLE_DATA].iteritems():
    #            row = self.yield_curve_table.table.find_row_by_index(self.maturity_map[maturity])
    #            row[YieldCurveFrame.COLUMNS[-4]] = data[YieldCurveFrame.COLUMNS[-4]]
    #            row[YieldCurveFrame.COLUMNS[-3]] = data[YieldCurveFrame.COLUMNS[-3]]
    #        self.yield_curve_table.redraw()
    #        self.set_time(shifted_data[REAL_TIME], shifted_data[USER_TIME], shifted_data[SYSTEM_TIME])
    
    def back_to_original(self):
        data = self.service.back_to_original()
        self.set_data(data)
    #        self.yield_curve_table.set_data(self.original_data,self.COLUMNS)
    #        self.yield_curve_table.redraw()
    #        self.set_time(data[REAL_TIME], data[USER_TIME], data[SYSTEM_TIME])
    
    def bump_curve(self,maturity,bump_yield_rate):
        bump_data = self.service.bump_curve(maturity,bump_yield_rate)
        self.set_data(bump_data)
    #        for maturity2,data in bump_data[TABLE_DATA].iteritems():
    #            row = self.yield_curve_table.table.find_row_by_index(self.maturity_map[maturity2])
    #            for index in range(1,len(YieldCurveFrame.COLUMNS)-1):
    #                row[YieldCurveFrame.COLUMNS[index]] = data[index-1]
    #                index+=1
    #        self.yield_curve_table.redraw()
    #        self.set_time(bump_data[REAL_TIME], bump_data[USER_TIME], bump_data[SYSTEM_TIME])
    
    def set_time(self,real_time,user_time,system_time):
        self.status_bar.setRealTime(real_time)
        self.status_bar.setUserTime(user_time)
        self.status_bar.setSystemTime(system_time)
    
    def set_data(self,data):
        table_data = self.transform_data(self.__transform_data__(data[TABLE_DATA]))
        self.yield_curve_table.set_data(table_data, self.COLUMNS)
        self.set_time(data[REAL_TIME], data[USER_TIME], data[SYSTEM_TIME])
    
    def __transform_data__(self,data):
        if data == None:
            return None
        self.maturity_map= {}
        index=0
        result = []
        for maturity in self.YEARS:
            if maturity in data:
                row = list(data[maturity])
                row.insert(0,maturity)
                row.append("")
                result.append(row)
                self.maturity_map[maturity] = index
                index+=1
        return result