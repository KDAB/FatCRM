from squish import *
import test

def createCampaign():    
    clickTab(waitForObject(":SugarCRM Client: admin@SugarCRM on localhost.qt_tabwidget_tabbar_QTabBar"), "Campaigns")    
    clickButton(waitForObject(":Form.New Campaign_QPushButton"))
    
def registerDetails(dList):                                                 
    type(waitForObject(":Details.name_QLineEdit_3"), dList[0]) 
    mouseClick(waitForObject(":Details.status_QComboBox"), 47, 18, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Details.status_QComboBox", dList[1]), 43, 11, 0, Qt.LeftButton)   
    #type(waitForObject(":Details.status_QComboBox"), dList[1])
    clickButton(waitForObject(":Details.Edit_EditCalendarButton_2"))
    waitForObjectItem(":Calendar.qt_calendar_calendarview_QCalendarView", dList[2])
    clickItem(":Calendar.qt_calendar_calendarview_QCalendarView", dList[2], 18, 8, 0, Qt.LeftButton)
    clickButton(waitForObject(":Details.Edit_EditCalendarButton_3"))
    waitForObjectItem(":Calendar.qt_calendar_calendarview_QCalendarView", dList[3])
    clickItem(":Calendar.qt_calendar_calendarview_QCalendarView", dList[3], 15, 10, 0, Qt.LeftButton)
    type(waitForObject(":Details.campaignType_QComboBox"), dList[4])
    type(waitForObject(":Details.currency_QComboBox"), dList[5])
    type(waitForObject(":Details.budget_QLineEdit"), dList[6])
    type(waitForObject(":Details.expectedRevenue_QLineEdit"), dList[7])    
    
def registerOtherDetails(oList): 
    type(waitForObject(":Other Details.assignedUserName_QComboBox"), oList[0])
    type(waitForObject(":Other Details.impressions_QLineEdit"), oList[1])
    type(waitForObject(":Other Details.actualCost_QLineEdit"), oList[2])
    type(waitForObject(":Other Details.expectedCost_QLineEdit"), oList[3])
    type(waitForObject(":Other Details.objective_QTextEdit"), oList[4])
    
def registerDescription(description):
    type(waitForObject(":Description:.content_QTextEdit"), description)
    
def saveCampaign():
    clickButton(waitForObject(":&Campaign Details.Save_QPushButton"))
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.menubar_QMenuBar", "File"))
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.File_QMenu", "Syncronize"))
    label = waitForObject(':Campaign Information.modifiedBy_QLabel')
    waitFor("label.text !=''")    
    
def checkDetailsValues(dList):
    test.compare(findObject(":Details.name_QLineEdit_3").text, dList[0])    
    test.compare(findObject(":Details.status_QComboBox").currentText, dList[1])
    if(dList[2] == "1/1"):
        test.compare(findObject(":Details.startDate_QLineEdit").text, "2010-07-25")
    if(dList[3] == "2/2"):    
        test.compare(findObject(":Details.endDate_QLineEdit").text, "2010-08-02")    
    test.compare(findObject(":Details.campaignType_QComboBox").currentText, dList[4])
    test.compare(findObject(":Details.currency_QComboBox").currentText, dList[5])
    test.compare(findObject(":Details.budget_QLineEdit").text, dList[6])
    test.compare(findObject(":Details.expectedRevenue_QLineEdit").text, dList[7])          
    
def checkOtherDetailsValues(oList):
    test.compare(findObject(":Other Details.assignedUserName_QComboBox").currentText, oList[0])
    test.compare(findObject(":Other Details.impressions_QLineEdit").text, oList[1])
    test.compare(findObject(":Other Details.actualCost_QLineEdit").text, oList[2])
    test.compare(findObject(":Other Details.expectedCost_QLineEdit").text, oList[3])
    test.compare(findObject(":Other Details.objective_QTextEdit").plainText, oList[4])
   
def checkDescriptionValue(description):
    test.compare(findObject(":Description:.content_QTextEdit").plainText, description)
    
def removeCampaign(name):      
    type(waitForObject(":Form.searchLE_QLineEdit_4"), name)    
    mouseClick((":Form.campaignsTV_Akonadi::EntityTreeView"), 67, 5, 0, Qt.LeftButton)
    clickButton(waitForObject(":Form.Remove Campaign_QPushButton"))
    clickButton(waitForObject(":SugarClient - Delete Campaign.Yes_QPushButton"))
    clickButton(waitForObject(":Form.Clear_QToolButton_4"))
    type(waitForObject(":Form.searchLE_QLineEdit_4"), name)  
    treeView = waitForObject(":Form.campaignsTV_Akonadi::EntityTreeView")
    model = treeView.model()
    count = model.rowCount()
    test.compare(count, 0) 
 