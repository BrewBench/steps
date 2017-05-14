angular.module('brewbench-steps')
.controller('mainCtrl', function($scope, $stateParams, $state, $filter, $timeout, $interval, $q, BrewService){

  $scope.showSettings = true;
  $scope.error_message = '';

  //default settings values
  $scope.settings = BrewService.settings('settings') || {
    unit: 'F'
    ,arduinoUrl: '192.168.240.1'
    ,ports: {'analog':5, 'digital':13}
    ,notifications: {on:true,slack:'Slack notification webhook Url',last:''}
    ,sounds: {on:true,alert:'/assets/audio/bike.mp3',timer:'/assets/audio/school.mp3'}
  };

  $scope.steps = BrewService.settings('steps') || [{
    name: 'Step'
    ,pin: 2
    ,analog: false
    ,running: false
    ,finished: false
    ,seconds: 5
  }];

  $scope.showSettingsSide = function(){
    $scope.showSettings = !$scope.showSettings;
  };

  $scope.clearSteps = function(e,i){
      angular.element(e.target).html('Removing...');
      BrewService.clear();
      $timeout(function(){
        window.location.reload();
      },1000);
  };

  $scope.addStep = function(){
      $scope.steps.push({
          name: 'Next Step'
          ,pin: 2
          ,analog: false
          ,running: false
          ,finished: false
          ,seconds: 5
        });
  };

  $scope.pinInUse = function(pin,analog){
    return _.find($scope.steps, function(step){
      return step.analog === analog && step.pin === pin;
    });
  };

  $scope.getPortRange = function(number){
      number++;
      return Array(number).fill().map((_, idx) => 0 + idx);
  };

  $scope.pinChange = function(old_pin,new_pin,analog){
    //find step with new pin and replace it with old pin
    var step = $scope.pinInUse(new_pin,analog);
    if(step && step.pin == new_pin){
        step.pin = old_pin;
    }
  };

});
