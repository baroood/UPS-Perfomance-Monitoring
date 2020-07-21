import MySQLdb

def dis_data():
	db=MySQLdb.connect('localhost','root','12345678','ESW')
	cursor = db.cursor()
	cursor.execute("""SELECT * FROM data_stored ;""")
	L=cursor.rowcount
	Exists = cursor.fetchall()
	cursor.close()
	print
	print('Database:')
	for i in Exists:
		print(i)
	db.close();

def avg_pow():
	db=MySQLdb.connect('localhost','root','12345678','ESW')
	cursor = db.cursor()
	cursor.execute("""SELECT AVG(PowerIN) FROM data_stored ;""")
	L=cursor.rowcount
	Exists = cursor.fetchall()
	cursor.close()
	print()
	print()
	print('Average Power input is:')
	for i in Exists:
		print("{0:.3f}".format(i[0]),'kWh')
	cursor = db.cursor()
	cursor.execute("""SELECT AVG(PowerOUT) FROM data_stored ;""")
	L=cursor.rowcount
	Exists = cursor.fetchall()
	cursor.close()
	print()
	print()
	print('Average Power output is:')
	for i in Exists:
		print("{0:.3f}".format(i[0]),'kWh')
		
	print()
	db.close();

def energy_loss():
	db=MySQLdb.connect('localhost','root','12345678','ESW')
	cursor = db.cursor()
	cursor.execute("""SELECT MAX(EnergyIN)-MAX(EnergyOUT) AS Energy_loss FROM data_stored;""")
	L=cursor.rowcount
	Exists = cursor.fetchall()
	cursor.close()
	print()
	print()
	print('Energy Loss:')
	for i in Exists:
		print("{0:.3f}".format(i[0]),'Wh')

	print()
	db.close();


def efficiency():
	db=MySQLdb.connect('localhost','root','12345678','ESW')
	cursor = db.cursor()
	cursor.execute("""SELECT (MAX(EnergyOUT)/MAX(EnergyIN)) AS Energy_loss FROM data_stored;""")
	L=cursor.rowcount
	Exists = cursor.fetchall()
	cursor.close()
	print()
	print()
	print('Efficiency of UPS is:')
	for i in Exists: 
		x=100*i[0]
		print()
		print("{0:.3f}".format(x),'%')

	print()
	db.close();


def UPS_states():
	db=MySQLdb.connect('localhost','root','12345678','ESW')
	cursor = db.cursor()
	cursor.execute("""SELECT PowerIn,PowerOUT FROM data_stored;""")
	L=cursor.rowcount
	Exists = cursor.fetchall()
	cursor.close()
	count1 = 0 
	count2 = 0 
	print()
	print()
	print('UPS is in discharging state (PowerIN < PowerOUT) for :')
	for i in Exists: 
		if float(i[1]) > float(i[0]):
			count1 += 1
		else:
			count2 += 1
	print(count1,'mins')

	db.close();


while True:
	print("1: Efficiency of UPS")
	print("2: Average Powers(IN and OUT)")
	print("3: Energy Loss")
	print("4: UPS discharge state Duration")
	print("5: Display data stored")
	print("6: Exit")
	choice = int(input("Enter your choice: "))

	if choice == 1:
		efficiency()

	elif choice == 2:
	 	avg_pow()

	elif choice == 3:
		energy_loss()

	elif choice == 4:
		UPS_states()

	elif choice == 5:
		dis_data()

	elif choice == 6:
		break

	else:
		print('Invalid input')

	print 
	print("--------------------------------------------------------------------------")
	print

print('Ba Bye!')