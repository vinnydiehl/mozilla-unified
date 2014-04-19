const Cc = Components.classes;
const Ci = Components.interfaces;
const Cu = Components.utils;

function run_test() {
  var secMan = Cc["@mozilla.org/scriptsecuritymanager;1"].getService(Ci.nsIScriptSecurityManager);
  do_check_true(secMan.isSystemPrincipal(Cu.getObjectPrincipal({})));
  var sb = new Cu.Sandbox('http://www.example.com');
  Cu.evalInSandbox('var obj = { foo: 42 };', sb);
  do_check_eq(Cu.getObjectPrincipal(sb.obj).origin, 'http://www.example.com');
}
