angular.module('brewbench-steps')
.factory('BrewService', function($http, $q, $filter){
  return {
    clear: function(){
      if(window.localStorage){
        window.localStorage.removeItem('settings');
        window.localStorage.removeItem('steps');
        window.localStorage.removeItem('urls');
      }
    },

    settings: function(key,values){
          if(!window.localStorage)
            return values;
          try {
            if(values){
              return window.localStorage.setItem(key,JSON.stringify(values));
            }
            else if(window.localStorage.getItem(key)){
              return JSON.parse(window.localStorage.getItem(key));
            }
          } catch(e){
            /*JSON parse error*/
          }
          return values;
    },

    domain: function(format){
      var settings = this.settings('settings');
      var domain = '';

      if(settings && settings.arduinoUrl)
        domain = settings.arduinoUrl.indexOf('//')===-1 ? '//'+settings.arduinoUrl : settings.arduinoUrl;
      else if(document.location.host == 'localhost')
        domain = '//arduino.local';

      if(!!format)
        return domain.indexOf('//')!==-1 ? domain.substring(domain.indexOf('//')+2) : domain;
      return domain;
    },

    slack: function(webhook_url,msg,color,icon,step){
      var q = $q.defer();

      var postObj = {'attachments': [{'fallback': msg,
            'title': step.name,
            'title_link': 'http://'+document.location.host+'/#/arduino/'+this.domain(true),
            'fields': [{'value': msg}],
            'color': color,
            'mrkdwn_in': ['text', 'fallback', 'fields'],
            'thumb_url': icon
          }]
        };

      $http({url: webhook_url, method:'POST', data:'payload='+JSON.stringify(postObj), headers: { 'Content-Type': 'application/x-www-form-urlencoded' }})
        .then(function(response){
          q.resolve(response.data);
        },function(err){
          q.reject(err);
        });
      return q.promise;
    },

    pkg: function(){
        var q = $q.defer();
        $http.get('/package.json').then(function(response){
          q.resolve(response.data);
        },function(err){
          q.reject(err);
        });
        return q.promise;
    },

    arduinoWrite: function(type,sensor,value){
      var q = $q.defer();
      var url = this.domain()+'/arduino/'+type+'/'+sensor+'/'+value;

      $http({url: url, method: 'GET'})
        .then(function(response){
          q.resolve(response.data);
        }, function(err){
          q.reject(err);
        });
      return q.promise;
    },

    arduinoRead: function(type,sensor,timeout){
      var q = $q.defer();
      var url = this.domain()+'/arduino/'+type+'/'+sensor;

      $http({url: url, method: 'GET'})
        .then(function(response){
          q.resolve(response.data);
        }, function(err){
          q.reject(err);
        });
      return q.promise;
    }

  };
});
