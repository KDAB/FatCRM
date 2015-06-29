def main():
    startApplication("fatcrm")
    # load methods     
    import campaignsHandling
    import leadsHandling
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
    # data to be registered
    firstName = "Lead" 
    detailsList = ["Self Generated", "This is a self generated lead", "", "Michel", "Mr.", firstName, "Bob", "2/2", "an account", "VD", "Sales", "max"]
    otherDetailsList = ["New", "this is a new lead", "150000", "123456", "789012", "345678", "901234", "567890", "true", "michel@kdab.com", "mic@kdab.se"]
    addressesList = ["jonas väg", "Hagfors", " a state","32456", "Sweden", "jonsons", "väg", "Örebro", "45678", "USA", "false"]   
    description = "a lead created via my refactored functions"
    # launch the details widget
    leadsHandling.createLead()
    # write the data
    leadsHandling.registerDetails(detailsList)
    leadsHandling.registerOtherDetails(otherDetailsList)
    leadsHandling.registerAddresses(addressesList)    
    leadsHandling.registerDescription(description)
    # save remotely
    leadsHandling.saveLead()
    # make sure it is saved before proceeding
    label = findObject(':Lead Information.modifiedDate_QLineEdit')
    waitFor("label.text !=''")
    # modified data     
    mdetailsList = ["Cold Call", "mThis is a self generated lead", "TestCampaign", "mMichel", "Mrs.", "mLead", "mBob", "3/3", "mMMMMan account", "mVD", "mSales", "max"]
    motherDetailsList = ["Assigned", "mmmmMthis is a new lead", "190000", "m123456", "m789012", "m345678", "m901234", "m567890", "false", "mmmmichel@kdab.com", "mmmic@kdab.se"]
    maddressesList = ["mmjonas väg", "mHagfors", " ma state","m32456", "mSweden", "mjonsons", "mväg", "mÖrebro", "m45678", "mUSA", "false"]   
    mdescription = "mmmmmMa lead created via my refactored functions"
    # write the data
    leadsHandling.registerDetails(mdetailsList)
    leadsHandling.registerOtherDetails(motherDetailsList)
    leadsHandling.registerAddresses(maddressesList)    
    leadsHandling.registerDescription(mdescription)
    # save remotely
    leadsHandling.saveLead()
    # make sure it is saved before proceeding
    label = findObject(':Lead Information.modifiedDate_QLineEdit')
    waitFor("label.text !=''")
    # check data values 
    leadsHandling.checkDetailsValues(mdetailsList)
    leadsHandling.checkOtherDetailsValues(motherDetailsList)
    leadsHandling.checkAddressesValues(maddressesList)   
    leadsHandling.checkDescriptionValue(mdescription)
    # remove 
    campaignsHandling.removeCampaign(name)  
    leadsHandling.removeLead(firstName)
    # quit
    mainWindowHandling.justQuit()