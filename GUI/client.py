#import Tkinter as tk

#class Application(tk.Frame):
#	def __init__(self, master=None):
#		tk.Frame.__init__(self, master)

#		self.grid()
#		self.createWidgets()
	
#	def createWidgets(self):
#		self.quitButton = tk.Button(self, text = 'Quit', command = self.quit)
#		self.quitButton.grid()

#app = Application()
#app.master.title('FSP')
#app.mainloop()

from Tkinter import *
class CellLabel(Label):
    def __init__(self,master,text=""):
        Label.__init__(self,master,bd=1,relief=SUNKEN,anchor=W)
        if len(text)>0:
            self.config(text=text)
    
    def set(self,format):
        self.config(text = format)
    
    def clear(self):
        self.config(text="")

class StatusBar(Frame):
    def __init__(self,master):
        Frame.__init__(self,master,relief=RIDGE)
        self.executionTimeLabel = CellLabel(self,"Execution time:")
        self.executionTimeLabel.grid(row = 0,column = 0,sticky="news")
        self.titleRealTimeLabel = CellLabel(self,"Real time:")
        self.titleRealTimeLabel.grid(row = 0,column = 1,sticky="news")
        self.titleUserTimeLabel = CellLabel(self,"User time:")
        self.titleUserTimeLabel.grid(row = 0,column = 2,sticky="news")
        self.titleSystemTimeLabel = CellLabel(self,"System time:")
        self.titleSystemTimeLabel.grid(row = 0,column = 3,sticky="news")
        
        self.emptyTimeLabel = CellLabel(self,"");
        self.emptyTimeLabel.grid(row = 1,column = 0,sticky="news")
        self.realTimeLabel = CellLabel(self,"")
        self.realTimeLabel.grid(row = 1,column = 1,sticky="news")
        self.userTimeLabel = CellLabel(self,"")
        self.userTimeLabel.grid(row = 1,column = 2,sticky="news")
        self.systemTimeLabel = CellLabel(self,"")
        self.systemTimeLabel.grid(row = 1,column = 3,sticky="news")
    
    def setRealTime(self,realtime):
        self.realTimeLabel.set(realtime)
    
    def setUserTime(self,usertime):
        self.userTimeLabel.set(usertime)
    
    def setSystemTime(self,systemtime):
        self.systemTimeLabel.set(systemtime)
    
    def clearRealTime(self):
        self.realTimeLabel.clear()
    
    def clearUserTime(self):
        self.userTimeLabel.clear()
    
    def clearSystemTime(self):
        self.systemTimeLabel.clear()



#for test
if __name__ == '__main__':
    root = Tk()
    statusBar = StatusBar(root)
    statusBar.pack(side=BOTTOM, fill=X)
    root.mainloop()
