def main():
    startApplication("sugarclient")
    # load methods 
    import contactsHandling
    import mainWindowHandling
    # data to be registered
    firstName = "TestContact" 
    detailsList = ["Mr.", firstName, "Bob", "Director Sales", "Admin", "", "Campaign","" ,"max", "", "michel@kdab.com"]
    otherDetailsList = ["123456", "789012", "345678", "901234", "567890", "3/3", "Mrs Assist", "567890", "true"]
    addressesList = ["jonas väg", "Hagfors", " a state","32456", "Sweden", "jonsons", "väg", "Örebro", "45678", "USA", "false"]   
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
    # check data values 
    contactsHandling.checkDetailsValues(detailsList)
    contactsHandling.checkOtherDetailsValues(otherDetailsList)
    contactsHandling.checkAddressesValues(addressesList)   
    contactsHandling.checkDescriptionValue(description)
    # remove opportunity
    contactsHandling.removeContact(firstName)
    # quit
    mainWindowHandling.justQuit()