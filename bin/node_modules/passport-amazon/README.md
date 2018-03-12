# Passport-Amazon

[Passport](https://github.com/jaredhanson/passport) strategy for authenticating
with [Amazon](http://www.amazon.com/) using the OAuth 2.0 API.

This module lets you authenticate using Amazon in your Node.js applications.  By
plugging into Passport, Amazon authentication can be easily and unobtrusively
integrated into any application or framework that supports
[Connect](http://www.senchalabs.org/connect/)-style middleware, including
[Express](http://expressjs.com/).

## Install

    $ npm install passport-amazon

## Usage

#### Configure Strategy

The Amazon authentication strategy authenticates users using an Amazon
account and OAuth 2.0 tokens.  The strategy requires a `verify` callback, which
accepts these credentials and calls `done` providing a user, as well as
`options` specifying a client ID, client secret, and callback URL.

The client ID and secret are obtained by registering an application at the
[Login with Amazon Developer Center](http://login.amazon.com/).

    passport.use(new AmazonStrategy({
        clientID: AMAZON_CLIENT_ID,
        clientSecret: AMAZON_CLIENT_SECRET,
        callbackURL: "http://127.0.0.1:3000/auth/amazon/callback"
      },
      function(accessToken, refreshToken, profile, done) {
        User.findOrCreate({ amazonId: profile.id }, function (err, user) {
          return done(err, user);
        });
      }
    ));

#### Authenticate Requests

Use `passport.authenticate()`, specifying the `'amazon'` strategy, to
authenticate requests.

For example, as route middleware in an [Express](http://expressjs.com/)
application:

    app.get('/auth/amazon',
      passport.authenticate('amazon'));

    app.get('/auth/amazon/callback', 
      passport.authenticate('amazon', { failureRedirect: '/login' }),
      function(req, res) {
        // Successful authentication, redirect home.
        res.redirect('/');
      });

## Examples

For a complete, working example, refer to the [login example](https://github.com/jaredhanson/passport-amazon/tree/master/examples/login).

## Tests

    $ npm install --dev
    $ make test

[![Build Status](https://secure.travis-ci.org/jaredhanson/passport-amazon.png)](http://travis-ci.org/jaredhanson/passport-amazon)

## Credits

  - [Jared Hanson](http://github.com/jaredhanson)

## License

[The MIT License](http://opensource.org/licenses/MIT)

Copyright (c) 2013 Jared Hanson <[http://jaredhanson.net/](http://jaredhanson.net/)>
