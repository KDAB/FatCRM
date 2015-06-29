def main():
    startApplication("fatcrm")
    # load methods 
    import campaignsHandling
    import accountsHandling
    import mainWindowHandling
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
    modifyDetailsList = ["test account", "mwww.kdab.net", "ma ticker", "test account" , "mAB", "Apparel", "Analyst", "TestCampaign", "max"]
    modifyOtherList = ["m12345", "m67890", "m09876", "m100", "m90", "m3333", "m1000000000", "m@kdab.com"]
    modifyBillingList = ["mbilling","mHagfors", "mVärmland", "m34567", "mSweden"]
    modifyShippingList = ["mshipping", "mHagfors", "mVärmland", "m34567", "mSweden"]
    modifyDescription = "a modified account description" 
    #modify data
    accountsHandling.registerDetails(modifyDetailsList)
    accountsHandling.registerOtherDetails(modifyOtherList)
    accountsHandling.registerAddresses(modifyBillingList, modifyShippingList)
    accountsHandling.registerDescription(modifyDescription)
    #save
    accountsHandling.saveAccount()
    # make sure it is saved before proceeding
    label = findObject(':Account Information.modifiedDate_QLineEdit')
    waitFor("label.text !=''")  
    #check
    accountsHandling.checkDetailsValues(modifyDetailsList)
    accountsHandling.checkOtherDetailsValues(modifyOtherList)
    accountsHandling.checkAddressesValues(modifyBillingList, modifyShippingList)
    accountsHandling.checkDescriptionValue(modifyDescription)
    #test done - cleanup and quit
    accountsHandling.removeAccount(accountName) 
    campaignsHandling.removeCampaign(name)  
    # quit
    mainWindowHandling.justQuit()