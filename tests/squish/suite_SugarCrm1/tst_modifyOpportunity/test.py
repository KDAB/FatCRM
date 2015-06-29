def main():
    startApplication("fatcrm")
    # load methods 
    import campaignsHandling
    import accountsHandling
    import opportunitiesHandling
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
    #create an opportunity
    # data to be registered
    opportunityName = "test opportunity"
    detailsList = [opportunityName, "", "Existing Business", "Employee", "", "Prospecting", "max"]
    otherDetailsList = ["US Dollars : $", "10000", "1/2", "another step", "90"]   
    description = "an opportunity created via my refactored functions"
    # launch the details widget
    opportunitiesHandling.createOpportunity()
    # write the data
    opportunitiesHandling.registerDetails(detailsList)
    opportunitiesHandling.registerOtherDetails(otherDetailsList)    
    opportunitiesHandling.registerDescription(description)
    # save remotely
    opportunitiesHandling.saveOpportunity()
    modifyList = [opportunityName, accountName, "Existing Business", "Employee", name, "Prospecting", "max"]
    #modify data
    opportunitiesHandling.registerDetails(modifyList)
    #save
    opportunitiesHandling.saveOpportunity()
    #check
    opportunitiesHandling.checkDetailsValues(modifyList)
    #test done - cleanup and quit
    accountsHandling.removeAccount(accountName) 
    campaignsHandling.removeCampaign(name)
    opportunitiesHandling.removeOpportunity(opportunityName)  
    #quit
    mainWindowHandling.justQuit()