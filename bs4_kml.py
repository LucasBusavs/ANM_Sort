from bs4 import BeautifulSoup as Soup

with open('P3.kml', encoding="utf8") as data:
    kml_soup = Soup(data, 'lxml-xml') # Parse as XML

descriptions = kml_soup.find_all('description')

phaseSet = set()

for i in range(1, len(descriptions)):
    description = descriptions[i]
    html_soup = Soup(description.text, 'lxml') # Parse as HTML
    
    tables = html_soup.find_all('table')
    
    table = tables[0]
    
    phaseCol = table.find_all('td')
    
    phaseType = phaseCol[12].text
    
    phaseSet.add(phaseType)

phaseSet = list(phaseSet)
phaseSet.sort()
print(phaseSet)