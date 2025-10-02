from flask import Flask, render_template, request, jsonify

app = Flask(__name__)
last_message = "No message received yet"

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/receive-message', methods=['POST'])
def receive_message():
    print('recieved POST request')
    global last_message
    data = request.get_json()
    if data and 'message' in data:
        last_message = data['message']
        return jsonify({"status": "success"})
    return jsonify({"status": "error", "message": "Invalid data format"})

@app.route('/get-message')
def get_message():
    return jsonify({"message": last_message})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)



