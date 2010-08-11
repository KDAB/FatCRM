from squish import *
import test

def selectTab(tabName):
    clickTab(waitForObject(":SugarCRM Client: admin@SugarCRM on localhost.qt_tabwidget_tabbar_QTabBar"), tabName)
