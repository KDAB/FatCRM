from squish import *
import test

def selectTab(tabName):
    clickTab(waitForObject(":SugarCRM Client: admin@SugarCRM on localhost.qt_tabwidget_tabbar_QTabBar"), tabName)

def justQuit():
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.menubar_QMenuBar", "File"))
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.File_QMenu", "Quit"))
    clickButton(waitForObject(":Quit Application.Quit_QPushButton"))