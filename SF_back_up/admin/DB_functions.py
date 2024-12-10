import hashlib
import os
from PIL import Image
import io
from flask import Flask
from flask_mysqldb import MySQL
import pandas as pd
import datetime
from flask_bcrypt import Bcrypt
import matplotlib.pyplot as plt

def getID(value, mod):
    hashObj = hashlib.sha256()
    hashObj.update(value.encode('utf-8'))
    hexHash= hashObj.hexdigest()
    ID = int(hexHash,16) % mod
    return ID



def DBconnect(app):
    app.secret_key = 'kjdjkshljKLKJKLWWjsqqqPOllmJA$%&*^&'
    app.config['MYSQL_HOST'] = '86.5.64.82'
    app.config['MYSQL_USER'] = 'faceRecUser'
    app.config['MYSQL_PASSWORD'] = 'ALTV_face_rec_admin3306'
    app.config['MYSQL_PORT'] = 3306
    app.config['MYSQL_DB'] = 'faceRecognition'
    mysql = MySQL(app)
    return mysql

def getCurrentEvent(db, studentID):
    cursor = db.connection.cursor()
    sql = "SELECT e.eventID FROM event e JOIN attendance a ON e.eventID = a.eventID WHERE a.studentID = %s AND e.eventStart < NOW() AND NOW() < DATE_ADD(eventStart,INTERVAL 1 HOUR);"
    cursor.execute(sql)
    return cursor.fetchone()


def MakeEvent(db,location,startTime,teacherName):
    teacherID = getID(teacherName,10000000)
    cursor = db.connection.cursor()
    cursor.execute("INSERT INTO event(eventID, location,eventStart,teacher) VALUES(%s,%s,%s,%s)",[getID(location+teacherName+startTime,10000000),location,startTime,teacherID])
    db.connection.commit()
    print(cursor.rowcount, "record inserted.")

def addSeminarGroupToEvent(db,eventID,seminarGroup):
    cursor = db.connection.cursor()
    cursor.execute("SELECT studentID FROM student WHERE seminarGroup = %s ",(seminarGroup,))
    students = cursor.fetchall()
    for student in students:
        sql = "INSERT INTO attendance(studentID,eventID,attended,timeArrived) VALUES(%s,%s,%s,%s)"
        cursor.execute(sql,(student[0],eventID,False,None))
        db.connection.commit()
    print(cursor.rowcount, "record inserted.")

def AddTeacher(db,teacherName, password):
    print(teacherName)
    cursor = db.connection.cursor()
    sql = "INSERT INTO teacher(teacherID,password)  VALUES(%s,%s)"
    try:
        cursor.execute(sql, (getID(teacherName,10000000), bcrypt.generate_password_hash(password=password)))
        db.connection.commit()
        print(cursor.rowcount, "record inserted.")
        return True
    except Exception as e:
        print("cannot record data error s%", e)
        return False
    
def validateTeacher(db,bcrypt,teacherName, password):
    cursor = db.connection.cursor()
    try:
        cursor.execute("SELECT * FROM teacher WHERE teacherID = %s;",(getID(teacherName,10000000),))
        account = cursor.fetchone()
        print(account[0])
        print(account[1])
        if account == None:
            print("No account found")
            return None
        if bcrypt.check_password_hash(account[1],password):
            return True
        else:
            return False
    except Exception as e:
        print('error: %s',e)


def updateAttendence(db,student,eventID):
    sql = "UPDATE attendance SET attended = %s,timeArrived = NOW() WHERE studentID = %s AND eventID = %s"
    values = (True,str(student),eventID,)
    cursor = db.connection.cursor()
    cursor.execute(sql,values)
    print(values)
    print(cursor.rowcount, "record ammended.")
    db.connection.commit()

def getStudentAttendanceStats(db,student):
    db = db.connection
    print(student)
    sql = "SELECT * FROM attendance WHERE studentID = " + str(student)
    df = pd.read_sql_query(sql = sql, con=db)
    print(df)
    return df


def addStudent(db,studentName,seminarGroup):
    cursor = db.connection.cursor()
    sql = "INSERT INTO student(studentID,studentName,seminarGroup) VALUES(%s,%s,%s)"
    if studentName == None:
        print("no student name added")
        return None
    else:
        studentID = getID(studentName,10000000)
        try:
            cursor.execute(sql,(studentID,studentName,seminarGroup))
            db.connection.commit()
            print(cursor.rowcount, "record inserted.")
            return True
        except Exception as e:
            print("cannot record data error s%", e)
            return False
        
def addSeminarGroup(db,seminarGroup):
    cursor = db.connection.cursor()
    sql = "INSERT INTO seminarGroup(groupID) VALUES(%s)"
    if seminarGroup == None:
        print("no student name added")
        return None
    else:
        try:
            cursor.execute(sql,(seminarGroup,))
            db.connection.commit()
            print(cursor.rowcount, "record inserted.")
            return True
        except Exception as e:
            print("cannot record data error s%", e)
            return False

def getAttendancePie(mysql,studentName):
    print("######### MAKING PIE CHART########")
    df = getStudentAttendanceStats(mysql,getID(studentName,10000000))
    print("######### FINISHED GETTING DATA########")
    attendance_count = df['attended'].value_counts()

    # Labels for the pie chart
    labels = ['Not Attended','Attended']

    # Create pie chart
    plt.figure(figsize=(7, 7))
    plt.pie(attendance_count, labels=labels, autopct='%1.1f%%', startangle=90, colors=['#66b3ff', '#ff6666'])
    plt.title('Student Attendance Distribution')
    plt.axis('equal')
    if not os.path.exists("images/attendance_pie_chart.png"):
        os.mkdir("images")
    os.chdir("images")
    plt.savefig('attendance_pie_chart.png', bbox_inches='tight')

def getStudentName(db,studentID):
    cur = db.connection.cursor()
    cur.execute(f"SELECT studentName from student WHERE studentID = {studentID};")
    name = cur.fetchall()[0][0]
    cur.close()
    return name.capitalize()

#app = Flask(__name__)
#bcrypt = Bcrypt(app)
#mysql = DBconnect(app)
#with app.app_context():
#    getStudentAttendanceStats(mysql,getID("thomas",10000000))
#    getAttendancePie(mysql,"jason")