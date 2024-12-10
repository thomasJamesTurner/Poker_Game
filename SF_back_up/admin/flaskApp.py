import DB_functions
from flask import Flask, render_template, request, redirect, url_for, session
from flask_bcrypt import Bcrypt


app = Flask(__name__)
bcrypt = Bcrypt(app)

mysql = DB_functions.DBconnect(app)

@app.route('/')
def home():
    return render_template('login.html')

@app.route('/login', methods=['POST'])
def login():
    username = request.form['username']
    password = request.form['password']

    with app.app_context():
        if DB_functions.validateTeacher(db = mysql,
                                        bcrypt = bcrypt,
                                        teacherName = username,
                                        password=password):
            return redirect(url_for('dashboard'))
        else:
            return "Invalid username or password"

@app.route('/register',methods = ['POST'])
def register():
    username = request.form['username']
    password = request.form['password']

    with app.app_context():
        if DB_functions.AddTeacher(db= mysql,teacherName=username,password=password):
            print("added teacher successfully")
        else:
            print("failed to add teacher %s",username)

@app.route('/dashboard')
def dashboard():
    if 'teacher_id' not in session:
        return redirect(url_for('home'))
    
    cur = mysql.connection.cursor()
    cur.execute("SELECT * FROM students")
    students = cur.fetchall()
    cur.close()

    return render_template('dashboard.html', students=students)

@app.route('/logout', methods=['GET'])
def logout():
    sessions.pop('teacher_id', None)
    return redirect(url_for('home'))

if __name__ == '__main__':
    app.run(debug=True)