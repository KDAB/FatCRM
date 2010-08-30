def main():
    startApplication("sugarclient")
    # load methods 
    import leadsHandling
    import mainWindowHandling
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
    # check data values 
    leadsHandling.checkDetailsValues(detailsList)
    leadsHandling.checkOtherDetailsValues(otherDetailsList)
    leadsHandling.checkAddressesValues(addressesList)   
    leadsHandling.checkDescriptionValue(description)
    # remove opportunity
    leadsHandling.removeLead(firstName)
    # quit
    mainWindowHandling.justQuit()
