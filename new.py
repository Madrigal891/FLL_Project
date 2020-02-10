import gspread
from oauth2client.service_account import ServiceAccountCredentials
import matplotlib.pyplot as plt
def next_available_row(sheet, cols_to_sample=2):
  # looks for empty row based on values appearing in 1st N columns
  cols = sheet.range(1, 1, sheet.row_count, cols_to_sample)
  return max([cell.row for cell in cols if cell.value]) + 1
scope = ['https://spreadsheets.google.com/feeds','https://www.googleapis.com/auth/drive']
creds = ServiceAccountCredentials.from_json_keyfile_name('client_secure.json', scope)#opens the json and extraxts key,and email
client = gspread.authorize(creds)#authorizes it
print('\nPick A,B or C:')
a=input()

sheet = client.open(a).sheet1
values_list = sheet.col_values(3)
date_list=sheet.col_values(1)
date_lists=date_list
val=values_list
i=1
last=0
jeff=0
gaming=[]
value=0
mark=1
yax=[]
pp=next_available_row(sheet)
while i+1<pp:
    if date_lists[i]!=jeff:
        gaming.append(mark)
        jeff=date_lists[i]
        yax.append(value/(i-last))
        value=0
        last=i
        mark+=1

    else:
        value+=int(val[i])
    i+=1



performance = [10,8,6,4,2,1]

plt.plot(gaming,yax,
         gaming,yax,'ok')

plt.show()
