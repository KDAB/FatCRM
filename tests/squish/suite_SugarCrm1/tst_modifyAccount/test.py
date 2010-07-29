
def main():
    startApplication("sugarclient")
    # load an existing account 
    mouseClick(waitForObject(":Form.searchLE_QLineEdit"), 169, 5, 0, Qt.LeftButton)
    type(waitForObject(":Form.searchLE_QLineEdit"), "test account")
    waitForObjectItem(":Form.accountsTV_Akonadi::EntityTreeView", "Sweden")
    clickItem(":Form.accountsTV_Akonadi::EntityTreeView", "Sweden", 70, 9, 0, Qt.LeftButton)
    clickButton(waitForObject(":SugarCRM Client: admin@SugarCRM on localhost.Show Details_QCheckBox"))
    # modify a few values
    mouseClick(waitForObjectItem(":Details.parentName_QComboBox", "test account"), 156, 11, 0, Qt.LeftButton)
    mouseClick(waitForObject(":Details.campaignName_QComboBox"), 192, 9, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Details.campaignName_QComboBox", "test campaign"), 171, 8, 0, Qt.LeftButton)
    # save 
    clickButton(waitForObject(":&Account Details.Save_QPushButton"))
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.menubar_QMenuBar", "File"))
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.File_QMenu", "Syncronize"))
    label = waitForObject(':Account Information.admin_QLabel_2')
    waitFor("label.text !=''")
    # data is back from remote client check modified values 
    clickButton(waitForObject(":Form.New Account_QPushButton"))
    clickButton(waitForObject(":Form.Clear_QToolButton"))
    mouseClick(waitForObject(":Form.searchLE_QLineEdit"), 331, 8, 0, Qt.LeftButton)
    type(waitForObject(":Form.searchLE_QLineEdit"), "test account")
    waitForObjectItem(":Form.accountsTV_Akonadi::EntityTreeView", "mail@kdab\\.com")
    clickItem(":Form.accountsTV_Akonadi::EntityTreeView", "mail@kdab\\.com", 77, 6, 0, Qt.LeftButton)
    waitFor("object.exists(':Details.parentName_QComboBox')")
    test.compare(findObject(":Details.parentName_QComboBox").currentText, "test account")
    waitFor("object.exists(':Details.campaignName_QComboBox')")
    test.compare(findObject(":Details.campaignName_QComboBox").currentText, "test campaign")
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.menubar_QMenuBar", "File"))
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.File_QMenu", "Quit"))
