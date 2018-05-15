import smtplib
from email.MIMEText import MIMEText
import argparse,textwrap,time


if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter, description="pyemailer is a command line tool to send email to users from a given email id..", epilog=textwrap.dedent('''\
        If you use this tool, please email me at :
        kailash.yadav@elucidata.io
        
        Have fun using pyemailer!
        '''))
    parser.add_argument("--to", help="to whom should this email go?", required=True)
    parser.add_argument("--url",  help="contents for your email", required=True)
    parser.add_argument("--login_email",  help="email with which this script will log in to the SMTP server", required=True)
    parser.add_argument("--login_password",  help="email with which this script will log in to the SMTP server", required=True)
    
    args = parser.parse_args()
    server = smtplib.SMTP('smtp.gmail.com', 587)
    server.starttls()
    # server.login("kailash.yadav@elucidata.io", "eminem9876")
    server.login(args.login_email, args.login_password)
    msg = MIMEText(args.url,'html')
    msg['Subject'] = 'Data successfully uploaded to Polly'
    server.sendmail(args.login_email, args.to, msg.as_string())
    server.quit()
