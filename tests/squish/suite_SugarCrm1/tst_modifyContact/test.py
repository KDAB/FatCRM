
def main():
    startApplication("sugarclient")
    # load methods 
    import accountsHandling
    import campaignsHandling
    import contactsHandling
    
    # create a campaign
    # data to be registered
    name = "TestCampaign" 
    status = "In Queue"
    detailsList = [name, status, "1/1", "2/2", "Radio", "US Dollars : $", "100000", "200000"]
    otherDetailsList = ["max", "20000", "85000", "100000", "radio marketting"]      
    description = "a campaign created via my refactored functions"
    # launch the details widget
    campaignsHandling.createCampaign()
    # write the data
    campaignsHandling.registerDetails(detailsList)
    campaignsHandling.registerOtherDetails(otherDetailsList)       
    campaignsHandling.registerDescription(description)
    # save remotely
    campaignsHandling.saveCampaign()
    # make sure it is saved before proceeding 
    label = findObject(':Campaign Information.modifiedDate_QLineEdit')
    waitFor("label.text !=''")    
    # select account tab
    # mainWindowHandling.selectTab("Account")
    # create an account 
    # data to be registered
    accountName = "test account"
    detailsList = [accountName, "www.kdab.com", "a ticker", "", "SARL", "Apparel", "Analyst", "", "max"]
    otherDetailsList = ["12345", "67890", "09876", "100", "90", "3333", "1000000000", "mail@kdab.com"]
    billingList = ["billing","Hagfors", "Värmland", "34567", "Sweden"]
    shippingList = ["shipping", "Hagfors", "Värmland", "34567", "Sweden"]
    description = "an account created via my refactored functions"
    # launch the details widget
    accountsHandling.createAccount()
    # write the data
    accountsHandling.registerDetails(detailsList)
    accountsHandling.registerOtherDetails(otherDetailsList)
    accountsHandling.registerAddresses(billingList, shippingList)
    accountsHandling.registerDescription(description)
    # save remotely
    accountsHandling.saveAccount()
    label = findObject(':Account Information.modifiedDate_QLineEdit')
    waitFor("label.text !=''")  
    # data to be registered
    firstName = "TestContact" 
    detailsList = ["Mr.", firstName, "Bob", "Director Sales", "Admin", "", "Campaign", "", "max", "", "michel@kdab.com"]
    otherDetailsList = ["123456", "789012", "345678", "901234", "567890", "3/3", "Mrs Assist", "567890", "true"]
    addressesList = ["jonas väg", "Hagfors", " a state","32456", "Sweden", "jonsons", "väg", "Örebro", "45678", "USA"]   
    description = "a contact created via my refactored functions"
    # launch the details widget
    contactsHandling.createContact()
    # write the data
    contactsHandling.registerDetails(detailsList)
    contactsHandling.registerOtherDetails(otherDetailsList)
    contactsHandling.registerAddresses(addressesList)    
    contactsHandling.registerDescription(description)
    # save remotely
    contactsHandling.saveContact()
    # make sure it is saved before proceeding 
    label = findObject(':Contact Information.modifiedDate_QLineEdit')
    waitFor("label.text !=''") 
    text = label.text  
    # modified values
    mdetailsList = ["Ms.", firstName, "mBob", "mDirector Sales", "mAdmin", "test account", "Employee", "TestCampaign", "max", firstName, "mmichel@kdab.com"]
    motherDetailsList = ["m123456", "m789012", "m345678", "m901234", "m567890", "3/3", "mMrs Assist", "m567890", "false"]
    maddressesList = ["mjonas väg", "mHagfors", " ma state","m32456", "mSweden", "mjonsons", "mväg", "mÖrebro", "m45678", "mUSA"]   
    mdescription = "a contact created via my refactored functions and modified blah blah"
    # write the data
    contactsHandling.registerDetails(mdetailsList)
    contactsHandling.registerOtherDetails(motherDetailsList)
    contactsHandling.registerAddresses(maddressesList)    
    contactsHandling.registerDescription(mdescription)
    # save remotely
    contactsHandling.saveContact()
    # make sure it is saved before proceeding 
    label = findObject(':Contact Information.modifiedDate_QLineEdit')
    waitFor("label.text != text")   
    
    # check data values 
    contactsHandling.checkDetailsValues(mdetailsList)
    contactsHandling.checkOtherDetailsValues(motherDetailsList)
    contactsHandling.checkAddressesValues(maddressesList)   
    contactsHandling.checkDescriptionValue(mdescription)
    # test done cleanup
    contactsHandling.removeContact(firstName)
    accountsHandling.removeAccount(accountName) 
    campaignsHandling.removeCampaign(name)  
    #quit
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.menubar_QMenuBar", "File"))
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.File_QMenu", "Quit"))
