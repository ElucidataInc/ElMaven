/**
 * Module dependencies.
 */
var util = require('util')
  , OAuth2Strategy = require('passport-oauth').OAuth2Strategy
  , InternalOAuthError = require('passport-oauth').InternalOAuthError;


/**
 * `Strategy` constructor.
 *
 * The Amazon authentication strategy authenticates requests by delegating to
 * Amazon using the OAuth 2.0 protocol.
 *
 * Applications must supply a `verify` callback which accepts an `accessToken`,
 * `refreshToken` and service-specific `profile`, and then calls the `done`
 * callback supplying a `user`, which should be set to `false` if the
 * credentials are not valid.  If an exception occured, `err` should be set.
 *
 * Options:
 *   - `clientID`      your Amazon application's client id
 *   - `clientSecret`  your Amazon application's client secret
 *   - `callbackURL`   URL to which Amazon will redirect the user after granting authorization
 *
 * Examples:
 *
 *     passport.use(new AmazonStrategy({
 *         clientID: '123-456-789',
 *         clientSecret: 'shhh-its-a-secret'
 *         callbackURL: 'https://www.example.net/auth/amazon/callback'
 *       },
 *       function(accessToken, refreshToken, profile, done) {
 *         User.findOrCreate(..., function (err, user) {
 *           done(err, user);
 *         });
 *       }
 *     ));
 *
 * @param {Object} options
 * @param {Function} verify
 * @api public
 */
function Strategy(options, verify) {
  options = options || {};
  options.authorizationURL = options.authorizationURL || 'https://www.amazon.com/ap/oa';
  options.tokenURL = options.tokenURL || 'https://api.amazon.com/auth/o2/token';
  
  OAuth2Strategy.call(this, options, verify);
  this.name = 'amazon';
}

/**
 * Inherit from `OAuth2Strategy`.
 */
util.inherits(Strategy, OAuth2Strategy);


/**
 * Retrieve user profile from Amazon.
 *
 * This function constructs a normalized profile, with the following properties:
 *
 *   - `provider`         always set to `amazon`
 *   - `id`
 *   - `username`
 *   - `displayName`
 *
 * @param {String} accessToken
 * @param {Function} done
 * @api protected
 */
Strategy.prototype.userProfile = function(accessToken, done) {
  this._oauth2.get('https://api.amazon.com/user/profile', accessToken, function (err, body, res) {
    if (err) { return done(new InternalOAuthError('failed to fetch user profile', err)); }
    
    try {
      var json = JSON.parse(body);
      
      var profile = { provider: 'amazon' };
      if (!json.Profile) {
        profile.id = json.user_id;
        profile.displayName = json.name;
        profile.emails = [{ value: json.email }];
      } else {
        profile.id = json.Profile.CustomerId;
        profile.displayName = json.Profile.Name;
        profile.emails = [{ value: json.Profile.PrimaryEmail }];
      }
      
      profile._raw = body;
      profile._json = json;
      
      done(null, profile);
    } catch(e) {
      done(e);
    }
  });
}


/**
 * Expose `Strategy`.
 */
module.exports = Strategy;
