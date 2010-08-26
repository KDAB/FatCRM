from squish import *
import test

def createContact():    
    clickTab(waitForObject(":SugarCRM Client: admin@SugarCRM on localhost.qt_tabwidget_tabbar_QTabBar"), "Contacts")        
    clickButton(waitForObject(":Contacts.New Contact_QPushButton"))
    
def registerDetails(dList):       
    type(waitForObject(":Details.salutation_QComboBox_2"), dList[0])
    type(waitForObject(":Details.firstName_QLineEdit_2"), "<Ctrl+A>")
    type(waitForObject(":Details.firstName_QLineEdit_2"), dList[1])
    type(waitForObject(":Details.lastName_QLineEdit_2"), "<Ctrl+A>")      
    type(waitForObject(":Details.lastName_QLineEdit_2"), dList[2])
    type(waitForObject(":Details.title_QLineEdit_2"), "<Ctrl+A>")
    type(waitForObject(":Details.title_QLineEdit_2"), dList[3])
    type(waitForObject(":Details.department_QLineEdit_2"), "<Ctrl+A>")    
    type(waitForObject(":Details.department_QLineEdit_2"), dList[4]) 
    if(dList[5] != ""):   
        type(waitForObject(":Details.accountName_QComboBox_2"), dList[5])       
    type(waitForObject(":Details.leadSource_QComboBox_3"), dList[6])
    if(dList[7] != ""):
        type(waitForObject(":Details.campaign_QComboBox"), dList[7]) 
    if(dList[8] != ""):       
        type(waitForObject(":Details.assignedTo_QComboBox"), dList[8])
    if(dList[9] != ""):
         type(waitForObject(":Details.reportsTo_QComboBox"), dList[9])
    type(waitForObject(":Details.primaryEmail_QLineEdit"), "<Ctrl+A>")      
    type(waitForObject(":Details.primaryEmail_QLineEdit"), dList[10])   
        
def registerOtherDetails(oList):    
    type(waitForObject(":Other Details.officePhone_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.officePhone_QLineEdit"), oList[0])
    type(waitForObject(":Other Details.mobilePhone_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.mobilePhone_QLineEdit"), oList[1])
    type(waitForObject(":Other Details.homePhone_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.homePhone_QLineEdit"), oList[2])
    type(waitForObject(":Other Details.otherPhone_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.otherPhone_QLineEdit"), oList[3])
    type(waitForObject(":Other Details.fax_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.fax_QLineEdit"), oList[4])
    clickButton(waitForObject(":Other Details.Edit_EditCalendarButton_2"))
    waitForObjectItem(":Calendar.qt_calendar_calendarview_QCalendarView", oList[5])
    clickItem(":Calendar.qt_calendar_calendarview_QCalendarView", oList[5], 20, 9, 0, Qt.LeftButton)
    type(waitForObject(":Other Details.assistant_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.assistant_QLineEdit"), oList[6])
    type(waitForObject(":Other Details.assistantPhone_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Other Details.assistantPhone_QLineEdit"), oList[7])
    #look for checkbox state
    box = findObject(':Other Details.doNotCall_QCheckBox')
    state = box.checked
    if(oList[8] == "true"):
        if(state != 1):   
            clickButton(waitForObject(":Other Details.doNotCall_QCheckBox")) 
    if(oList[8] == "false"):
        if(state == 1):
            clickButton(waitForObject(":Other Details.doNotCall_QCheckBox"))
def registerAddresses(aList):
    type(waitForObject(":Addresses.primaryAddress_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Addresses.primaryAddress_QLineEdit"), aList[0])    
    type(waitForObject(":Addresses.city_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Addresses.city_QLineEdit"), aList[1])
    type(waitForObject(":Addresses.state_QLineEdit"), "<Ctrl+A>")    
    type(waitForObject(":Addresses.state_QLineEdit"), aList[2])
    type(waitForObject(":Addresses.postalCode_QLineEdit"), "<Ctrl+A>")        
    type(waitForObject(":Addresses.postalCode_QLineEdit"), aList[3])
    type(waitForObject(":Addresses.country_QLineEdit"), "<Ctrl+A>")    
    type(waitForObject(":Addresses.country_QLineEdit"), aList[4])
    type(waitForObject(":Addresses.otherAddress_QLineEdit"), "<Ctrl+A>")    
    type(waitForObject(":Addresses.otherAddress_QLineEdit"), aList[5])
    type(waitForObject(":Addresses.otherCity_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Addresses.otherCity_QLineEdit"), aList[6])
    type(waitForObject(":Addresses.otherState_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Addresses.otherState_QLineEdit"), aList[7])
    type(waitForObject(":Addresses.otherPostalCode_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Addresses.otherPostalCode_QLineEdit"), aList[8])
    type(waitForObject(":Addresses.otherCountry_QLineEdit"), "<Ctrl+A>")
    type(waitForObject(":Addresses.otherCountry_QLineEdit"), aList[9])   
 
def registerDescription(description):
    type(waitForObject(":Description:.description_QTextEdit_4"), "<Ctrl+A>")
    type(waitForObject(":Description:.description_QTextEdit_4"), description)
    
def saveContact():
    clickButton(waitForObject(":&Contact Details.Save_QPushButton"))    
    label = findObject(':Contact Information.modifiedDate_QLineEdit')
    waitFor("label.text !=''")    
    
def checkDetailsValues(dList):    
    test.compare(findObject(":Details.salutation_QComboBox_2").currentText, dList[0])
    test.compare(findObject(":Details.firstName_QLineEdit_2").text, dList[1])      
    test.compare(findObject(":Details.lastName_QLineEdit_2").text, dList[2])
    test.compare(findObject(":Details.title_QLineEdit_2").text, dList[3])    
    test.compare(findObject(":Details.department_QLineEdit_2").text, dList[4])    
    test.compare(findObject(":Details.accountName_QComboBox_2").currentText, dList[5])       
    test.compare(findObject(":Details.leadSource_QComboBox_3").currentText, dList[6])
    test.compare(findObject(":Details.campaign_QComboBox").currentText, dList[7])    
    test.compare(findObject(":Details.assignedTo_QComboBox").currentText, dList[8])
    test.compare(findObject(":Details.reportsTo_QComboBox").currentText, dList[9]) 
    test.compare(findObject(":Details.primaryEmail_QLineEdit").text, dList[10])    
    
def checkOtherDetailsValues(oList):
    test.compare(findObject(":Other Details.officePhone_QLineEdit").text, oList[0])
    test.compare(findObject(":Other Details.mobilePhone_QLineEdit").text, oList[1])
    test.compare(findObject(":Other Details.homePhone_QLineEdit").text, oList[2])
    test.compare(findObject(":Other Details.otherPhone_QLineEdit").text, oList[3])
    test.compare(findObject(":Other Details.fax_QLineEdit").text, oList[4])
    if(oList[5] == "3/3"):
        test.compare(findObject(":Other Details.birthDate_QLineEdit_2").text, "2010-08-10")    
    test.compare(findObject(":Other Details.assistant_QLineEdit").text, oList[6])
    test.compare(findObject(":Other Details.assistantPhone_QLineEdit").text, oList[7])
    if(oList[8] == "true"):
        test.compare(findObject(":Other Details.doNotCall_QCheckBox").checked, True )
    else:
        test.compare(findObject(":Other Details.doNotCall_QCheckBox").checked, False )              
    
def checkAddressesValues(aList):
    test.compare(findObject(":Addresses.primaryAddress_QLineEdit").text, aList[0])    
    test.compare(findObject(":Addresses.city_QLineEdit").text, aList[1])    
    test.compare(findObject(":Addresses.state_QLineEdit").text, aList[2])        
    test.compare(findObject(":Addresses.postalCode_QLineEdit").text, aList[3])    
    test.compare(findObject(":Addresses.country_QLineEdit").text, aList[4])    
    test.compare(findObject(":Addresses.otherAddress_QLineEdit").text, aList[5])    
    test.compare(findObject(":Addresses.otherCity_QLineEdit").text, aList[6])    
    test.compare(findObject(":Addresses.otherState_QLineEdit").text, aList[7])    
    test.compare(findObject(":Addresses.otherPostalCode_QLineEdit").text, aList[8])    
    test.compare(findObject(":Addresses.otherCountry_QLineEdit").text, aList[9])   
        
def checkDescriptionValue(description):
    test.compare(findObject(":Description:.description_QTextEdit_4").plainText, description)
    
def removeContact(firstName):  
    clickTab(waitForObject(":SugarCRM Client: admin@SugarCRM on localhost.qt_tabwidget_tabbar_QTabBar"), "Contacts")    
    type(waitForObject(":Contacts.searchLE_QLineEdit"), firstName)    
    mouseClick((":Contacts.contactsTV_Akonadi::EntityTreeView"), 20, 42, 0, Qt.LeftButton)
    clickButton(waitForObject(":Contacts.Remove Contact_QPushButton"))
    clickButton(waitForObject(":SugarClient - Delete Contact.Yes_QPushButton"))
    clickButton(waitForObject(":Contacts.Clear_QToolButton"))
    type(waitForObject(":Contacts.searchLE_QLineEdit"), firstName)
    treeView = waitForObject(":Contacts.contactsTV_Akonadi::EntityTreeView")
    model = treeView.model()
    count = model.rowCount()
    test.compare(count, 0) 
 