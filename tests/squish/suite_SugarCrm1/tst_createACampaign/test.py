def main():
    startApplication("sugarclient")
    # load methods 
    import campaignsHandling
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
    # check data values 
    campaignsHandling.checkDetailsValues(detailsList)
    campaignsHandling.checkOtherDetailsValues(otherDetailsList)       
    campaignsHandling.checkDescriptionValue(description)
    # remove 
    campaignsHandling.removeCampaign(name)
    # quit
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.menubar_QMenuBar", "File"))
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.File_QMenu", "Quit"))
