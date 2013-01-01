from Tkinter import *
import ttk
import style_initializer
from tab_container import TabContainer
from frame.trading_book_frame import TradingBookFrame
from frame.yield_curve_frame import YieldCurveFrame
from frame.pnl_distribution_frame import PnlDistriutionFrame
import Tkinter
import tkMessageBox
import socket
import constants
import TABLE_DATA

class MainWindow(ttk.Frame):
    '''
        classdocs
        '''
    
    ABOUT_MESSAGE =("Hello, this application is developped by Ze Wang,zw448@nyu.edu and Di,Li\n"
                    "version1: 12/07/2012")
    
    def __init__(self,master,service,**kw):
        '''
            Constructor
            '''
        ttk.Frame.__init__(self, master,**kw)
        self.service = service
        
        self.menu_bar = self.createMenuBar()
        self.tab_container = TabContainer(self)
        self.set_up_trading_book()
        self.set_up_yield_curve()
        self.set_up_pnl_distribution()
        
        self.master.minsize(1000, 400)
        self.tab_container.pack(fill=X, expand = True)
    
    def set_up_socket(self,domain,port):
        self.domain = domain
        self.port = port
        self.socket = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        self.socket.connect((domain,port))
    
    def set_up_trading_book(self):
        data = self.service.fetching_trading_book_data()
        self.trading_book_page = TradingBookFrame(self.tab_container,data)
        self.tab_container.add_page(self.trading_book_page,text = "Trading Book",sticky="nsew")
    
    def set_up_yield_curve(self):
        data = self.service.fetching_yield_curve_data()
        self.yield_curve_page = YieldCurveFrame(self.tab_container,data,self.service)
        self.tab_container.add_page(self.yield_curve_page,text = "Yield Curve",sticky="nsew")
    
    def reset_trading_book(self):
        data = self.service.fetching_trading_book_data()
        self.trading_book_page.set_data(data)
    
    def reset_yield_curve(self):
        data = self.service.fetching_yield_curve_data()
        self.yield_curve_page.set_data(data)
    
    def set_up_pnl_distribution(self):
        data = self.service.fetching_pnl_distribution_data()
        self.pnl_distribution_page = PnlDistriutionFrame(self.tab_container,data)
        self.tab_container.add_page(self.pnl_distribution_page,text = "PnL distribution",sticky="nsew")
    
    def reset_pnl_distribution(self):
        data = self.service.fetching_pnl_distribution_data()
        self.pnl_distribution_page.set_data(data)
    
    def about(self):
        tkMessageBox.showinfo("Financial Software Project", self.ABOUT_MESSAGE)
        pass
    
    
    def help(self):
        pass
    
    def reconnect(self):
        self.service.reconnect()
        self.reset_trading_book()
        self.reset_yield_curve()
        self.reset_pnl_distribution()
    
    def createMenuBar(self):
        menubar = Frame(self,relief=RAISED,borderwidth=1)
        menubar.pack(fill=X)
        
        mb_server = Menubutton(menubar,text='Server')
        mb_server.pack(side=LEFT)
        mb_server.menu = Menu(mb_server)
        
        mb_server.menu.add_command(label='Re-connect', command = self.reconnect)
        mb_server.menu.add_command(label='About', command = self.about)
        
        
        mb_help = Menubutton(menubar,text='Help')
        mb_help.pack(padx=25,side=LEFT)
        mb_help.menu = Menu(mb_help)
        mb_help.menu.add_command(label = "Help",command = self.help)
        
        mb_server['menu'] = mb_server.menu
        mb_help['menu'] = mb_help.menu
        return menubar

def start_main_window(service):
    root = Tkinter.Tk()
    style_initializer.init_styles()
    main = MainWindow(root,service)
    main.pack(fill='both', expand = True)
    root.mainloop()
