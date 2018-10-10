#!/usr/bin/env node

var chalk = require('chalk');
var fs = require("fs");
var request = require("request");
var AWS = require('aws-sdk');
var AmazonCognitoIdentity = require('amazon-cognito-identity-js');
var public_token_header = '';
var jwt_decode = require('jwt-decode');
var DNS = require('dns')
module.exports.hello = function() {
    console.log(chalk.green.bold("Hello from the other side! :) "));
}

module.exports.check_internet_connection = function(){
    // assuming google server will always be up.
    DNS.resolve('www.google.com', function(err) {
        if (err) {
            console.log("No connection");
        } else {
            console.log("Connected");
        }
        });
}

function isloggedin(){
    console.log(chalk.red.bold("Not loggedin"));
}


function write_id_token(token_filename,idtoken) {
    var fso = require("fs");
    var buf = new Buffer(idtoken);
    fso.unlink(token_filename, (err) => {
        if (err) {
            var cred_file = fs.openSync(token_filename, 'wx');
            fso.writeSync(cred_file, buf);
        }
        else {
            console.log(chalk.green.bold("no error"));
            var cred_file = fs.openSync(token_filename, 'wx');
            fso.writeSync(cred_file, buf);
        }
    });

}


function has_id_token(token_filename) {
    var fso = require("fs");

    if(!fso.existsSync(token_filename)){
        return false;
    }
    return true;
}


function read_id_token(token_filename){
    var fso = require("fs");
    var saved_token = fso.readFileSync(token_filename);
    return saved_token;    
}

function refreshToken(token_filename,email){
    if (!email) {
        console.log(chalk.red.bold("Email is required to refresh the token"));
        return;
    } else {
        if (!(email.includes("@"))){
            console.log(chalk.red.bold("Please provide valid email.."));
            return;
            }
        }
    console.log(chalk.yellow.bold("Fetching user pool to refresh token..."));
    
    var options = {
        method: 'GET',
        url: 'https://polly.elucidata.io/userpool',
        json: true
    };

    var cognito_client_id = "";
    var cognito_user_pool = "";
    var cognito_user_pool_region = "";

    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        console.log(chalk.yellow.bgBlack.bold("UserPool response: "));
        cognito_client_id = body.cognito_client_id;
        cognito_user_pool = body.cognito_user_pool;
        cognito_user_pool_region = body.cognito_user_pool_region;
        
        var poolData = {
            UserPoolId : cognito_user_pool,
            ClientId : cognito_client_id
        };
        var userPool = new AmazonCognitoIdentity.CognitoUserPool(poolData);
        var userData = {
            Username : email,
            Pool : userPool
        };
        
        var cognitoUser = new AmazonCognitoIdentity.CognitoUser(userData);
        var refresh_token = String(read_id_token(token_filename+"_refreshToken"))
        console.log("trying to refresh the token now..");
        var refresh_token_object = new AmazonCognitoIdentity.CognitoRefreshToken({ RefreshToken: refresh_token });
    
        cognitoUser.refreshSession(refresh_token_object, (err, session) => {
            if(err) {
                console.log("error while refreshing the token.."+err);
            } 
            else{
                write_id_token(token_filename, String(session.getIdToken().getJwtToken()));
                return;
            }
        });
        
    });
}


module.exports.authenticate = function(token_filename,email, password){
    if (has_id_token(token_filename) && has_id_token(token_filename+"_refreshToken")) {
        var public_token_header = read_id_token(token_filename);
        var decoded = jwt_decode(String(public_token_header));
        var token_expiry_date = decoded.exp.valueOf();
        var nowDate = new Date().getTime()/1000;
        // console.log(token_expiry_date,nowDate);
        if (token_expiry_date > nowDate) {
            console.log(chalk.green.bold("already logged in"));
            return;
        }
        else{
            return refreshToken(token_filename,decoded.email.valueOf());
        }
    }
    if (!email) {
        console.log(chalk.red.bold("Email is required param."));
        return;
    } else {
        if (!(email.includes("@"))){
            console.log(chalk.red.bold("First param is email. Second param is password."));
            return;
            }
        }
    if (!password) {
        console.log(chalk.red.bold("Password is required param."));
        return;
    }
    console.log(chalk.yellow.bold("Fetching user pool..."));
    var options = {
        method: 'GET',
        url: 'https://polly.elucidata.io/userpool',
        json: true
    };

    var cognito_client_id = "";
    var cognito_user_pool = "";
    var cognito_user_pool_region = "";
    var CognitoUserPool = AmazonCognitoIdentity.CognitoUserPool;

    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        console.log(chalk.yellow.bgBlack.bold("UserPool response: "));
        cognito_client_id = body.cognito_client_id;
        cognito_user_pool = body.cognito_user_pool;
        cognito_user_pool_region = body.cognito_user_pool_region;
        
        var authenticationData = {
            Username : email,
            Password : password,
        };
        var authenticationDetails = new AmazonCognitoIdentity.AuthenticationDetails(authenticationData);
        // var authenticationDetails = new AWS.CognitoIdentityServiceProvider.AuthenticationDetails(authenticationData);
        var poolData = {
            UserPoolId : cognito_user_pool,
            ClientId : cognito_client_id
        };
        var userPool = new AmazonCognitoIdentity.CognitoUserPool(poolData);
        var userData = {
            Username : email,
            Pool : userPool
        };
        
        var cognitoUser = new AmazonCognitoIdentity.CognitoUser(userData);
        var loginidp = 'cognito-idp.ap-southeast-1.amazonaws.com/' + cognito_user_pool;
        cognitoUser.authenticateUser(authenticationDetails, {
            onSuccess: function (result) {
                write_id_token(token_filename,String(result.getIdToken().getJwtToken()));
                AWS.config.region = cognito_user_pool_region;
                write_id_token(token_filename+"_refreshToken",String(result.getRefreshToken().getToken()))
            
                AWS.config.credentials = new AWS.CognitoIdentityCredentials({
                    IdentityPoolId : cognito_client_id,
                    Logins : {
                        loginidp: result.getIdToken().getJwtToken()
                    }
                });
            },
            onFailure: function (result) {
                console.log(chalk.red.bgBlack.bold('Error while logging in. Please check your credentials on the web app.'));
            }
        });
    });
    return;
}

module.exports.createWorkflowRequest = function (token_filename, project_id) {
    if (has_id_token(token_filename)) {
        public_token_header = read_id_token(token_filename);
    }
    var payload = {
        "workflow_details":{
            "workflow_name": "relative_lcms_elmaven",
            "workflow_id": 4
        },
        "name": "PollyPhi™ Relative LCMS El-MAVEN Untitled Untitled",
        "project_id": project_id
    }

    var options = {
        method: 'PUT',
        url: 'https://polly.elucidata.io/api/wf-request',
        headers:
            {
                'cache-control': 'no-cache',
                'content-type': 'application/json',
                'public-token': public_token_header
            },
        body:
            {
                payload: JSON.stringify(payload)
            },
        json: true
    };
    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        console.log(chalk.yellow.bgBlack.bold(`createWorkflowRequest Response: `));
        if ((response.statusCode != 200) && (response.statusCode != 400)) {
            console.log(chalk.red.bold("Unable to create workflow request id. Please authenticate. Status code:"));
            console.log(chalk.red.bold(response.statusCode));
            console.log(chalk.green.bold(JSON.stringify(body)));
            return;
        }
        console.log(chalk.green.bold(JSON.stringify(body)));
        return body
    });
}

module.exports.createProject = function (token_filename,name) {
    if (has_id_token(token_filename)) {
        public_token_header = read_id_token(token_filename);
    }
    var payload = {
        "name": name
    }
    var options = {
        method: 'POST',
        url: 'https://polly.elucidata.io/api/project',
        headers:
            {
                'cache-control': 'no-cache',
                'content-type': 'application/json',
                'public-token': public_token_header
            },
        body:
            {
                payload: JSON.stringify(payload)
            },
        json: true
    };
    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        console.log(chalk.yellow.bgBlack.bold(`createProject Response: `));
        if ((response.statusCode != 200) && (response.statusCode != 400)) {
            console.log(chalk.red.bold("Unable to create Project. Please authenticate. Status code:"));
            console.log(chalk.red.bold(response.statusCode));
            console.log(chalk.green.bold(JSON.stringify(body)));
            return;
        }
        console.log(chalk.green.bold(JSON.stringify(body)));
        return body
    });
}

module.exports.send_email = function (user_email,email_content,email_message) {
    if (!user_email) {
        console.log(chalk.red.bold("Email is required param."));
        return;
    } else {
        if (!(user_email.includes("@"))){
            console.log(chalk.red.bold("First param is email. Second param is email content. Third param is email_message. "));
            return;
            }
        }
    if (!email_content) {
        console.log(chalk.red.bold("email_content is required param."));
        return;
    }
    if (!email_message) {
        console.log(chalk.red.bold("email_message is required param."));
        return;
    }
    var options = {
        method: 'POST',
        url: ' https://7w9r94dq3h.execute-api.ap-south-1.amazonaws.com/cpj_beta/pyemail',
        headers:
            {
                'content-type': 'application/json'
            },
        body:
            {
                user_email:user_email,
                email_content:email_content,
                email_message:email_message
            },
        json: true
    };
    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        console.log(chalk.yellow.bgBlack.bold(`send email Response: `));
        if ((response.statusCode != 200) && (response.statusCode != 400)) {
            console.log(chalk.red.bold("Unable to send email. Please check your APIs. Status code:"));
            console.log(chalk.red.bold(response.statusCode));
            console.log(chalk.green.bold(JSON.stringify(body)));
            return;
        }
        console.log(chalk.green.bold(JSON.stringify(body)));
        return body
    });
}

module.exports.shareProject = function (token_filename,project_id,permission,usernames) {
    if (has_id_token(token_filename)) {
        public_token_header = read_id_token(token_filename);
    }
    var payload = {
        "project_sharing_list":
        [{"user_email":usernames,"project_id":project_id,"permission":permission}],
        "state":"share_project"
    }
    var options = {
        method: 'POST',
        url: 'https://polly.elucidata.io/api/sharing/share_project',
        headers:
            {
                'cache-control': 'no-cache',
                'content-type': 'application/json',
                'public-token': public_token_header
            },
        body:
            {
                payload: JSON.stringify(payload)
            },
        json: true
    };
    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        console.log(chalk.yellow.bgBlack.bold(`shareProject Response: `));
        if ((response.statusCode != 200) && (response.statusCode != 400)) {
            console.log(chalk.red.bold("Unable to share Project. Please authenticate. Status code:"));
            console.log(chalk.red.bold(response.statusCode));
            console.log(chalk.green.bold(JSON.stringify(body)));
            return;
        }
        console.log(chalk.green.bold(JSON.stringify(body)));
        return body
    });
}

module.exports.get_upload_Project_urls = function (token_filename,id) {
    if (has_id_token(token_filename)) {
        public_token_header = read_id_token(token_filename);
    }
    var options = {
        method: 'GET',
        url: 'https://polly.elucidata.io/api/project',
        headers:
            {
                'cache-control': 'no-cache',
                'content-type': 'application/json',
                'public-token': public_token_header                
            },
        body:
            {
                id: id,
                state:"get_upload_urls"
            },
        json: true
    };

    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        console.log(chalk.yellow.bgBlack.bold(`PostRun Response: `));
        if (response.statusCode != 200) {
            console.log(chalk.red.bold("Unable to get Project upload urls. Please authenticate. Status code:"));
            console.log(chalk.red.bold(response.statusCode));
            return;
        }
        console.log(chalk.green.bold(JSON.stringify(body)));
        return body
    });
}


module.exports.get_Project_names = function (token_filename) {
    if (has_id_token(token_filename)) {
        public_token_header = read_id_token(token_filename);
    }
    var options = {
        method: 'GET',
        url: 'https://polly.elucidata.io/api/project',
        headers:
            {
                'cache-control': 'no-cache',
                'content-type': 'application/json',
                'public-token': public_token_header                
            },
        body:
            {
            },
        json: true
    };

    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        console.log(chalk.yellow.bgBlack.bold(`PostRun Response: `));
        if (response.statusCode != 200) {
            console.log(chalk.red.bold("Unable to get Project upload urls. Please authenticate. Status code:"));
            console.log(chalk.red.bold(response.statusCode));
            return;
        }
        console.log(chalk.green.bold(JSON.stringify(body)));
        return body
    });
}

module.exports.get_organizational_databases = function (token_filename,organization) {
    if (has_id_token(token_filename)) {
        public_token_header = read_id_token(token_filename);
    }
    var options = {
        method: 'GET',
        url: 'https://polly.elucidata.io/api/project',
        headers:
            {
                'cache-control': 'no-cache',
                'content-type': 'application/json',
                'public-token': public_token_header                
            },
        body:
            {
                // put state or something here, that will return all the names of compound DBs stored in 
                // organization folder of Elmaven-Polly-Integration bucket..
            },
        json: true
    };

    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        console.log(chalk.yellow.bgBlack.bold(`PostRun Response: `));
        if (response.statusCode != 200) {
            console.log(chalk.red.bold("Unable to get Project upload urls. Please authenticate. Status code:"));
            console.log(chalk.red.bold(response.statusCode));
            return;
        }
        console.log(chalk.green.bold(JSON.stringify(body)));
        return body
    });
}

module.exports.get_Project_files = function (token_filename,id) {
    if (has_id_token(token_filename)) {
        public_token_header = read_id_token(token_filename);
    }
    var options = {
        method: 'GET',
        url: 'https://polly.elucidata.io/api/project',
        headers:
            {
                'cache-control': 'no-cache',
                'content-type': 'application/json',
                'public-token': public_token_header                
            },
        body:
            {
                id: id,
            },
        json: true
    };

    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        console.log(chalk.yellow.bgBlack.bold(`PostRun Response: `));
        if (response.statusCode != 200) {
            console.log(chalk.red.bold("Unable to get Project upload urls. Please authenticate. Status code:"));
            console.log(chalk.red.bold(response.statusCode));
            return;
        }
        console.log(chalk.green.bold(JSON.stringify(body)));
        return body
    });
}


module.exports.createPutRequest = function (token_filename,url, filePath) {
    if (has_id_token(token_filename)) {
        public_token_header = read_id_token(token_filename);
    }
    var options = {
        method: 'PUT',
        url: url,
        headers:
            {
                'cache-control': 'no-cache',
                'x-amz-acl': 'bucket-owner-full-control',
                'content-type': 'application/x-www-form-urlencoded',
                'public-token': public_token_header                
            },
        body: fs.readFileSync(filePath)
    };

    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        console.log(chalk.yellow.bgBlack.bold(`createPutRequest Response: `));
        if (response.statusCode != 200) {
            console.log(chalk.red.bold("Unable to create Run. Please authenticate. Status code:"));
            console.log(chalk.red.bold(response.statusCode));
            return;
        }
        console.log(chalk.green.bold(response.statusCode));
    });
}


module.exports.upload_project_data = function (url, filePath) {
    var options = {
        method: 'PUT',
        url: url,
        headers:
            {
                'x-amz-acl': 'bucket-owner-full-control',                
            },
        body: fs.readFileSync(filePath)
    };

    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        if (response.statusCode != 200) {
            console.log(chalk.red.bold("Unable to post project data.Status code:"));
            console.log(chalk.red.bold(response.statusCode));
            return;
        }
        console.log(chalk.green.bold(response.statusCode));
    });
}

module.exports.download_project_data = function (url, filePath) {
    var options = {
        method: 'GET',
        url: url,
        headers:
            {
                'x-amz-acl': 'bucket-owner-full-control',                
            },
        // body: {

        // }
    };

    request(options, function (error, response, body) {
        if (error) throw new Error(chalk.bold.red(error));
        if (response.statusCode != 200) {
            console.log(chalk.red.bold("Unable to get project data.Status code:"));
            console.log(chalk.red.bold(response.statusCode));
            return;
        }
        dataToWrite = response.body
        fs.writeFile(filePath, dataToWrite, 'utf8', function (err) {
            if (err) {
              console.log('Some error occured - file either not saved or corrupted file saved.');
            } else{
              console.log('It\'s saved!');
            }
        });
        console.log(chalk.green.bold(response.statusCode));
    });
}


require('make-runnable/custom')({
    printOutputFrame: false
})
