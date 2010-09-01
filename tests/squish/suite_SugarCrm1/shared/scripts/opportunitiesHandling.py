from squish import *
import test

def createOpportunity():
    clickTab(waitForObject(":SugarCRM Client: admin@SugarCRM on localhost.qt_tabwidget_tabbar_QTabBar"), "Opportunities")
    clickButton(waitForObject(":Page.New_QPushButton"))
   
def registerDetails(dList):    
    type(waitForObject(":Details.name_QLineEdit_2"), "<Ctrl+A>")
    type(waitForObject(":Details.name_QLineEdit_2"), dList[0])
    mouseClick(waitForObject(":Details.accountName_QComboBox"), 75, 5, 0, Qt.LeftButton)
    if(dList[1] != ""):     
        type(waitForObject(":Details.accountName_QComboBox"), dList[1]) 
    type(waitForObject(":Details.opportunityType_QComboBox"), dList[2])
    type(waitForObject(":Details.leadSource_QComboBox"), dList[3])
    if(dList[4] != ""):
        type(waitForObject(":Details.campaignName_QComboBox_2"), dList[4])
    type(waitForObject(":Details.salesStage_QComboBox"), dList[5])
    type(waitForObject(":Details.assignedUserName_QComboBox_2"), dList[6])
    
def registerOtherDetails(oList):
    type(waitForObject(":Other Details.currency_QComboBox"), oList[0])
    type(waitForObject(":Other Details.amount_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.amount_QLineEdit"), oList[1])
    clickButton(waitForObject(":Other Details.Edit_EditCalendarButton"))
    waitForObjectItem(":Calendar.qt_calendar_calendarview_QCalendarView", oList[2])
    clickItem(":Calendar.qt_calendar_calendarview_QCalendarView", oList[2], 15, 15, 0, Qt.LeftButton)
    type(waitForObject(":Other Details.nextStep_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.nextStep_QLineEdit"), oList[3])
    type(waitForObject(":Other Details.probability_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.probability_QLineEdit"), oList[4])
 
def registerDescription(description):
    type(waitForObject(":Description:.description_QTextEdit_2"), "<Ctrl+A>")
    type(waitForObject(":Description:.description_QTextEdit_2"), description)
    
def saveOpportunity():
    clickButton(waitForObject(":&Opportunity Details.Save_QPushButton"))    
    label = findObject(':Opportunity Information.modifiedDate_QLineEdit')
    waitFor("label.text !=''")
    
def checkDetailsValues(dList):    
    test.compare(findObject(":Details.name_QLineEdit_2").text, dList[0])        
    test.compare(findObject(":Details.accountName_QComboBox").currentText, dList[1])        
    test.compare(findObject(":Details.opportunityType_QComboBox").currentText, dList[2])        
    test.compare(findObject(":Details.leadSource_QComboBox").currentText, dList[3])  
    test.compare(findObject(":Details.campaignName_QComboBox_2").currentText, dList[4])      
    test.compare(findObject(":Details.salesStage_QComboBox").currentText, dList[5])        
    test.compare(findObject(":Details.assignedUserName_QComboBox_2").currentText, dList[6])
    
def checkOtherDetailsValues(oList):
    test.compare(findObject(":Other Details.currency_QComboBox").currentText, oList[0])    
    test.compare(findObject(":Other Details.amount_QLineEdit").text, oList[1])
    if(oList[2] == "1/2"):
        test.compare(findObject(":Other Details.dateClosed_QLineEdit").text, "2010-07-26")    
    test.compare(findObject(":Other Details.nextStep_QLineEdit").text, oList[3])    
    test.compare(findObject(":Other Details.probability_QLineEdit").text, oList[4])
    
def checkDescriptionValue(description):
    test.compare(findObject(":Description:.description_QTextEdit_2").plainText, description)
    
def removeOpportunity(opportunityName):    
    clickTab(waitForObject(":SugarCRM Client: admin@SugarCRM on localhost.qt_tabwidget_tabbar_QTabBar"), "Opportunities")
    type(waitForObject(":Form.searchLE_QLineEdit_2"), opportunityName)
    mouseClick(waitForObject(":Form.opportunitiesTV_Akonadi::EntityTreeView"), 135, 9, 0, Qt.LeftButton)
    clickButton(waitForObject(":Form.Remove Opportunity_QPushButton"))
    clickButton(waitForObject(":SugarClient - Delete Opportunity.Yes_QPushButton"))
    clickButton(waitForObject(":Form.Clear_QToolButton_2"))
    type(waitForObject(":Form.searchLE_QLineEdit_2"), opportunityName)
    treeView = waitForObject(":Form.opportunitiesTV_Akonadi::EntityTreeView")
    model = treeView.model()
    count = model.rowCount()
    test.compare(count, 0) 
   