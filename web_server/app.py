from flask import Flask
from flask_mail import Mail, Message

app = Flask(__name__)

# ---------------- Email Config ----------------
app.config['MAIL_SERVER'] = 'smtp.gmail.com'
app.config['MAIL_PORT'] = 465
app.config['MAIL_USE_SSL'] = True
app.config['MAIL_USERNAME'] = 'piconrad22@gmail.com'
app.config['MAIL_PASSWORD'] = 'untc fppx cprx ffsh'  # <-- use Gmail App Password
app.config['MAIL_DEFAULT_SENDER'] = 'piconrad22@gmail.com'

mail = Mail(app)

@app.route('/send-email')
def send_email():
    try:
        msg = Message("Hello World!", recipients=["piconrad22@gmail.com"])
        msg.body = "Hello world! This is a test email sent from Flask."
        mail.send(msg)
        return {"status": "Email sent successfully!"}
    except Exception as e:
        return {"status": "Failed", "error": str(e)}

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
