
def main():
    startApplication("sugarclient")
    # load methods 
    import accountsHandling
    import mainWindowHandling
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
    # check data values 
    accountsHandling.checkDetailsValues(detailsList)
    accountsHandling.checkOtherDetailsValues(otherDetailsList)
    accountsHandling.checkAddressesValues(billingList, shippingList)
    accountsHandling.checkDescriptionValue(description)
    # remove account
    accountsHandling.removeAccount(accountName)
    # quit
    mainWindowHandling.justQuit()
