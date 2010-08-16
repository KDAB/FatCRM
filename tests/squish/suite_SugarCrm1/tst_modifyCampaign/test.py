def main():
    startApplication("sugarclient")
    # load methods 
    import campaignsHandling   
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
    # new data
    mDetailsList = [name, status, "2/2", "3/3", "Radio", "US Dollars : $", "500000", "6600000"]
    mOtherDetailsList = ["max", "60000", "9000000", "300000", "mradio marketting"]      
    mDescription = "a modified campaign"
    #modify 
    campaignsHandling.registerDetails(mDetailsList)
    campaignsHandling.registerOtherDetails(mOtherDetailsList)       
    campaignsHandling.registerDescription(mDescription)
    # save remotely
    campaignsHandling.saveCampaign()    
    # make sure it is saved before proceeding 
    label = findObject(':Campaign Information.modifiedDate_QLineEdit')
    waitFor("label.text !=''")    
    #verify
    campaignsHandling.checkDetailsValues(mDetailsList)
    campaignsHandling.checkOtherDetailsValues(mOtherDetailsList)
    campaignsHandling.checkDescriptionValue(mDescription)
    #remove
    campaignsHandling.removeCampaign(name)
    
    