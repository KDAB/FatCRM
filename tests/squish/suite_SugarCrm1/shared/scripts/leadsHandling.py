from squish import *
import test

def createLead():
    clickTab(waitForObject(":SugarCRM Client: admin@SugarCRM on localhost.qt_tabwidget_tabbar_QTabBar"), "Leads")
    clickButton(waitForObject(":Page.New_QPushButton"))
          
def registerDetails(dList):   
    type(waitForObject(":Details.leadSource_QComboBox_2"), dList[0])   
    type(waitForObject(":Details.leadSourceDescription_QTextEdit"), "<Ctrl+A>")
    type(waitForObject(":Details.leadSourceDescription_QTextEdit"), dList[1])   
    if(dList[2] != ""):
        type(waitForObject(":Details.campaignName_QComboBox_3"), dList[2])
    type(waitForObject(":Details.referedBy_QLineEdit"), "<Ctrl+A>")    
    type(waitForObject(":Details.referedBy_QLineEdit"), dList[3])
    type(waitForObject(":Details.salutation_QComboBox"), dList[4])
    type(waitForObject(":Details.firstName_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Details.firstName_QLineEdit"), dList[5])
    type(waitForObject(":Details.lastName_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Details.lastName_QLineEdit"), dList[6])
    clickButton(waitForObject(":Details.Edit_EditCalendarButton"))
    waitForObjectItem(":Calendar.qt_calendar_calendarview_QCalendarView", dList[7])
    clickItem(":Calendar.qt_calendar_calendarview_QCalendarView", dList[7], 13, 6, 0, Qt.LeftButton)
    type(waitForObject(":Details.accountName_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Details.accountName_QLineEdit"), dList[8])
    type(waitForObject(":Details.title_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Details.title_QLineEdit"), dList[9])
    type(waitForObject(":Details.department_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Details.department_QLineEdit"), dList[10])
    type(waitForObject(":Details.assignedUserName_QComboBox_3"), dList[11])
    
def registerOtherDetails(oList):
    type(waitForObject(":Other Details.status_QComboBox"), oList[0])
    type(waitForObject(":Other Details.statusDescription_QTextEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.statusDescription_QTextEdit"), oList[1])
    type(waitForObject(":Other Details.opportunityAmount_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.opportunityAmount_QLineEdit"), oList[2])
    type(waitForObject(":Other Details.phoneWork_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.phoneWork_QLineEdit"), oList[3])
    type(waitForObject(":Other Details.phoneMobile_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.phoneMobile_QLineEdit"), oList[4])
    type(waitForObject(":Other Details.phoneHome_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.phoneHome_QLineEdit"), oList[5])
    type(waitForObject(":Other Details.phoneOther_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.phoneOther_QLineEdit"), oList[6])
    type(waitForObject(":Other Details.phoneFax_QLineEdit_2"), "<Ctrl+A>")
    type(waitForObject(":Other Details.phoneFax_QLineEdit_2"), oList[7])
    if(oList[8] == "true"):
        clickButton(waitForObject(":Other Details.doNotCall_QCheckBox_2"))
    type(waitForObject(":Other Details.email1_QLineEdit_2"), "<Ctrl+A>")
    type(waitForObject(":Other Details.email1_QLineEdit_2"), oList[9])
    type(waitForObject(":Other Details.email2_QLineEdit"), "<Ctrl+A>")    
    type(waitForObject(":Other Details.email2_QLineEdit"), oList[10])
    
def registerAddresses(aList):
    type(waitForObject(":Addresses.primaryAddressStreet_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Addresses.primaryAddressStreet_QLineEdit"), aList[0])
    type(waitForObject(":Addresses.primaryAddressCity_QLineEdit"), "<Ctrl+A>")    
    type(waitForObject(":Addresses.primaryAddressCity_QLineEdit"), aList[1])
    type(waitForObject(":Addresses.primaryAddressState_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Addresses.primaryAddressState_QLineEdit"), aList[2])    
    type(waitForObject(":Addresses.primaryAddressPostalcode_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Addresses.primaryAddressPostalcode_QLineEdit"), aList[3])
    type(waitForObject(":Addresses.primaryAddressCountry_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Addresses.primaryAddressCountry_QLineEdit"), aList[4])
    type(waitForObject(":Addresses.altAddressStreet_QLineEdit"), "<Ctrl+A>")    
    type(waitForObject(":Addresses.altAddressStreet_QLineEdit"), aList[5])
    type(waitForObject(":Addresses.altAddressCity_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Addresses.altAddressCity_QLineEdit"), aList[6]) 
    type(waitForObject(":Addresses.altAddressState_QLineEdit"), "<Ctrl+A>")   
    type(waitForObject(":Addresses.altAddressState_QLineEdit"), aList[7])
    type(waitForObject(":Addresses.altAddressPostalcode_QLineEdit"), "<Ctrl+A>")    
    type(waitForObject(":Addresses.altAddressPostalcode_QLineEdit"), aList[8])
    type(waitForObject(":Addresses.altAddressCountry_QLineEdit"), "<Ctrl+A>")    
    type(waitForObject(":Addresses.altAddressCountry_QLineEdit"), aList[9])
    if(aList[10] == "true"):
        clickButton(waitForObject(":Addresses.copyAddressFromPrimary_QCheckBox"))        
 
def registerDescription(description):
    type(waitForObject(":Description:.description_QTextEdit_3"), "<Ctrl+A>")
    type(waitForObject(":Description:.description_QTextEdit_3"), description)
    
def saveLead():
    clickButton(waitForObject(":&Lead Details.Save_QPushButton"))    
    label = findObject(':Lead Information.modifiedDate_QLineEdit')
    waitFor("label.text !=''")
    
def checkDetailsValues(dList):    
    test.compare(findObject(":Details.leadSource_QComboBox_2").currentText, dList[0])        
    test.compare(findObject(":Details.leadSourceDescription_QTextEdit").plainText, dList[1])         
    test.compare(findObject(":Details.campaignName_QComboBox_3").currentText, dList[2])    
    test.compare(findObject(":Details.referedBy_QLineEdit").text, dList[3])        
    test.compare(findObject(":Details.salutation_QComboBox").currentText, dList[4])
    test.compare(findObject(":Details.firstName_QLineEdit").text, dList[5])
    test.compare(findObject(":Details.lastName_QLineEdit").text, dList[6])
    if(dList[7] == "2/2"):
         test.compare(findObject(":Details.birthdate_QLineEdit_2").text, "2010-08-02")
    test.compare(findObject(":Details.accountName_QLineEdit").text, dList[8])                                  
    test.compare(findObject(":Details.title_QLineEdit").text, dList[9])
    test.compare(findObject(":Details.department_QLineEdit").text, dList[10]) 
    test.compare(findObject(":Details.assignedUserName_QComboBox_3").currentText, dList[11])
    
def checkOtherDetailsValues(oList):
    test.compare(findObject(":Other Details.status_QComboBox").currentText, oList[0])    
    test.compare(findObject(":Other Details.statusDescription_QTextEdit").plainText, oList[1])   
    test.compare(findObject(":Other Details.opportunityAmount_QLineEdit").text, oList[2])    
    test.compare(findObject(":Other Details.phoneWork_QLineEdit").text, oList[3])
    test.compare(findObject(":Other Details.phoneMobile_QLineEdit").text, oList[4])
    test.compare(findObject(":Other Details.phoneHome_QLineEdit").text, oList[5])
    test.compare(findObject(":Other Details.phoneOther_QLineEdit").text, oList[6])
    test.compare(findObject(":Other Details.phoneFax_QLineEdit_2").text, oList[7])
    if(oList[8] == "true"):
        test.compare(findObject(":Other Details.doNotCall_QCheckBox_2").checked, True)
    else:
        test.compare(findObject(":Other Details.doNotCall_QCheckBox_2").checked, False)    
    test.compare(findObject(":Other Details.email1_QLineEdit_2").text, oList[9])
    test.compare(findObject(":Other Details.email2_QLineEdit").text, oList[10])                    
    
def checkAddressesValues(aList):
    test.compare(findObject(":Addresses.primaryAddressStreet_QLineEdit").text, aList[0])    
    test.compare(findObject(":Addresses.primaryAddressCity_QLineEdit").text, aList[1])    
    test.compare(findObject(":Addresses.primaryAddressState_QLineEdit").text, aList[2])    
    test.compare(findObject(":Addresses.primaryAddressPostalcode_QLineEdit").text, aList[3])
    test.compare(findObject(":Addresses.primaryAddressCountry_QLineEdit").text, aList[4])    
    test.compare(findObject(":Addresses.altAddressStreet_QLineEdit").text, aList[5])    
    test.compare(findObject(":Addresses.altAddressCity_QLineEdit").text, aList[6])    
    test.compare(findObject(":Addresses.altAddressState_QLineEdit").text, aList[7])    
    test.compare(findObject(":Addresses.altAddressPostalcode_QLineEdit").text, aList[8])    
    test.compare(findObject(":Addresses.altAddressCountry_QLineEdit").text, aList[9])
    if(aList[10] == "true"):
        test.compare(findObject(":Addresses.copyAddressFromPrimary_QCheckBox").checked, True)
    else:
        test.compare(findObject(":Addresses.copyAddressFromPrimary_QCheckBox").checked, False)
        
def checkDescriptionValue(description):
    test.compare(findObject(":Description:.description_QTextEdit_3").plainText, description)
    
def removeLead(firstName):      
    clickTab(waitForObject(":SugarCRM Client: admin@SugarCRM on localhost.qt_tabwidget_tabbar_QTabBar"), "Leads")
    type(waitForObject(":Form.searchLE_QLineEdit_3"), firstName)    
    mouseClick((":Form.leadsTV_Akonadi::EntityTreeView"), 62, 8, 0, Qt.LeftButton)
    clickButton(waitForObject(":Form.Remove Lead_QPushButton"))
    clickButton(waitForObject(":SugarClient - Delete Lead.Yes_QPushButton"))
    clickButton(waitForObject(":Form.Clear_QToolButton_3"))
    type(waitForObject(":Form.searchLE_QLineEdit_3"), firstName)
    treeView = waitForObject(":Form.leadsTV_Akonadi::EntityTreeView")
    model = treeView.model()
    count = model.rowCount()
    test.compare(count, 0) 
 