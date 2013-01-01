from ttk import Frame
from Tkinter import *
from ui.other_widget.status_bar import *
from ui.table.data_table import *
from ui.frame.data_frame import MyDataFrame
from constants import constants
from constants.constants import *

class SummaryBar(Frame):
    def __init__(self,master):
        Frame.__init__(self, master,relief=RIDGE)
        self.titleVarSpread = CellLabel(self,VAR_SPREAD)
        self.titleVarSpread.grid(row = 0,column = 0,sticky="news")
        self.titleVarYield = CellLabel(self,VAR_YIELD)
        self.titleVarYield.grid(row = 0,column = 1,sticky="news")
        self.titleVarPrice = CellLabel(self,VAR_PRICE)
        self.titleVarPrice.grid(row = 0,column = 2,sticky="news")
        self.titleVarTotal = CellLabel(self,TOTAL)
        self.titleVarTotal.grid(row = 0,column = 3,sticky="news")
        self.varSpread = CellLabel(self,"")
        self.varSpread.grid(row = 1,column = 0,sticky="news")
        self.varYield = CellLabel(self,"")
        self.varYield.grid(row = 1,column = 1,sticky="news")
        self.varPrice = CellLabel(self,"")
        self.varPrice.grid(row = 1,column = 2,sticky="news")
        self.varTotal = CellLabel(self,"")
        self.varTotal.grid(row = 1,column = 3,sticky="news")
    
    def set_var_spread(self,var_spread):
        self.varSpread.set(var_spread)
    
    def set_var_yield(self,var_yield):
        self.varYield.set(var_yield)
    
    def set_var_price(self,var_price):
        self.varPrice.set(var_price)
    
    def set_var_total(self,var_total):
        self.varTotal.set(var_total)
    
    def clear_var_spread(self,var_spread):
        self.varSpread.clear()
    
    def clear_var_yield(self,var_yield):
        self.varYield.clear()
    
    def clear_var_price(self,var_price):
        self.varPrice.clear()
    
    def clear_var_total(self,var_total):
        self.varTotal.clear()

class TradingBookFrame(MyDataFrame):
    '''
        classdocs
        '''
    COLUMNS = ["Ticker","Notional","Risk","LGD","Notional1","Risk1","LGD1","Notional2","Risk2","LGD2"]
    UPPER_COLUMN_HEADERS = [{'text':'','colspan':1},
                            {'text':'Day 1','colspan':3}
                            ,{'text':'Day 2','colspan':3}
                            ,{'text':'Change ','colspan':3}]
    QUALITIES = ['AAA','AA','A','BBB','BB','B','CCC','CC','C','D']
    
    VARS = [VAR_SPREAD,VAR_YIELD,TOTAL]
    
    def __init__(self,master,data,**kw):
        '''
            Constructor
            '''
        MyDataFrame.__init__(self, master,**kw)
        self.pack(fill=BOTH)
        self.trading_book_table = DataTable(self,self.transform_data(self.__transform_data__(data[TABLE_DATA])),
                                            TradingBookFrame.COLUMNS,TradingBookFrame.UPPER_COLUMN_HEADERS,
                                            width=1250,height=500,editable=False)
        
        
        #self.trading_book_table.pack(fill=BOTH,side=TOP)
        self.trading_book_table.grid(row = 0,column=0,columnspan=2,sticky='news')
        #        self.summary_bar = SummaryBar(self)
        #        self.summary_bar.grid(row = 1,column=0,sticky='news')
        self.status_bar = StatusBar(self)
        #self.status_bar.pack(fill=BOTH,side=BOTTOM)
        self.status_bar.grid(row = 1,column=1,sticky='news')
        if data != None:
            self.set_data(data)
    
    def set_data(self,data):
        table_data = self.transform_data(self.__transform_data__(data[TABLE_DATA]))
        self.trading_book_table.set_data(table_data, self.COLUMNS)
        self.set_time(data[REAL_TIME], data[USER_TIME], data[SYSTEM_TIME])
    #        self.set_summary(data[TABLE_DATA][VAR_SPREAD],data[TABLE_DATA][VAR_YIELD],data[TABLE_DATA][VAR_PRICE],data[TABLE_DATA][TOTAL])
    
    def set_time(self,real_time,user_time,system_time):
        self.status_bar.setRealTime(real_time)
        self.status_bar.setUserTime(user_time)
        self.status_bar.setSystemTime(system_time)
    
    #    def set_summary(self,var_spread,var_yield,var_price,var_total):
    #        self.summary_bar.set_var_spread(var_spread)
    #        self.summary_bar.set_var_yield(var_yield)
    #        self.summary_bar.set_var_price(var_price)
    #        self.summary_bar.set_var_total(var_total)
    
    def __transform_data__(self,table_data):
        if table_data == None:
            return None
        data = []
        
        ticker_data = table_data[constants.TICKERS]
        for ticker,row in ticker_data.iteritems():
            row = list(row)
            row.insert(0,ticker)
            data.append(row)
    
        quality_data = table_data[constants.QUAILITY]
        for quality in self.QUALITIES:
            if quality in quality_data:
                row = list(quality_data[quality])
                row.insert(0,quality)
                data.append(row)
    
        var_data = table_data[constants.VAR]
        
        for var_cat in self.VARS:
            if var_cat in var_data:
                row = list(var_data[var_cat])
                row.insert(0,var_cat)
                data.append(row)
        return data
