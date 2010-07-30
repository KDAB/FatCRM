def main():
    startApplication("sugarclient")
    # load methods 
    import opportunitiesHandling
    # data to be registered
    opportunityName = "test opportunity"
    detailsList = [opportunityName, "", "Existing Business", "Employee", "", "Prospecting", "max"]
    otherDetailsList = ["US Dollars:$", "10000", "1/2", "another step", "90"]   
    description = "an opportunity created via my refactored functions"
    # launch the details widget
    opportunitiesHandling.createOpportunity()
    # write the data
    opportunitiesHandling.registerDetails(detailsList)
    opportunitiesHandling.registerOtherDetails(otherDetailsList)    
    opportunitiesHandling.registerDescription(description)
    # save remotely
    opportunitiesHandling.saveOpportunity()
    # check data values 
    opportunitiesHandling.checkDetailsValues(detailsList)
    opportunitiesHandling.checkOtherDetailsValues(otherDetailsList)   
    opportunitiesHandling.checkDescriptionValue(description)
    # remove opportunity
    opportunitiesHandling.removeOpportunity(opportunityName)
    # quit
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.menubar_QMenuBar", "File"))
    activateItem(waitForObjectItem(":SugarCRM Client: admin@SugarCRM on localhost.File_QMenu", "Quit"))
