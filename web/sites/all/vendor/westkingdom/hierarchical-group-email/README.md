## Contents

1. [Hierarchical Group Mailing List Management](#group-management)
1. [Running the Tests](#running-the-tests)
1. [Basic Example](#basic-example)
  1. [Include this Library Using Composer](#composer-instructions)
  1. [Configuring Your Authentication Information](#configuring-your-authentication-information)
  1. [Prepare Your Data](#prepare-your-data)
1. [Expanded Example](#expanded-example)
  1. [Create a Service Authenticator](#create-a-service-authenticator)
  1. [Authenticate](#authenticate)
  1. [Create a Standard Group Policy](#create-a-standard-group-policy)
  1. [Create a Google Apps Group Controller](#create-a-google-apps-group-controller)
  1. [Create a Groups Object and Update It](#create-a-groups-object-and-update-it)
  1. [Debugging, logging or prompting](#debugging-logging-or-prompting)

## Group Management

This library assists in the management of mailing lists for hierarchical
groups.  It is presumed that group memberships themselves will be managed
in some external system (e.g. Drupal); the mailing lists themselves are
created as Google Groups within a Google Groups for Business (or Nonprofits,
or Education) account.

The assumption is that the group heirarchies are organized by regions (for
example, State, County and City); each region is called a "branch".  Each
branch has a number of offices, and each office can have multiple officers
(e.g. the office holder and assistants, jointly-held offices, and so on).
Every branch office is a group, and every group has exactly one email list.
If an office requires multiple lists, this can be accomplished by creating
sub-offices, as the offices can be hierarchical as well.  This is only necessary
when the group membership for each list needs to be vary, though, as every
mailing list can have any number of alternate addressess, any of which can
be used to send mail to the list.

Updates to groups are made in bulk.  The update process goes something like this:

- Membership data is provided via a PHP associative array that
defines the group names and member email addresses.
- It is expected that the caller will also provide a similar array, containing a cached representation of the group membership data from the last time an update
was made.  
- The update function in this class will then call the Google API to make any necessary additions or deletions from the group membership lists.

When the update function runs, it will create, update and remove groups
and group memberships as needed in order to make the Google Groups memberships
exactly match the memberships described in the provided input arrays.  In
addition to these groups, a number of "aggregate" groups are also automatically
created at the same time.  An "aggregate" group is a group whose members are
entirely composed of other groups.  There are three kinds of aggregate groups
created:

- All officers of a type:  An aggregated list is created for every unique type of officer in the system.  For example, a "presidents" list is created to address all of the presidents of every branch.
- Branch officers:  An aggregated list is created for every branch in the system.  This "officers" list will send email to every officer in that branch.
- Custom aggregated lists:  The caller can define collections of officers that can be messaged at a single address.  For example, an "executive" group could be defined to contain the president, finance officer and secretary; in this instance, every branch would be given an "executive" mailing list that would send email to these officers.

These aggregate groups will be managed automatically.

## Running the Tests

This library contains a test suite that uses PHPUnit and Prophecy to
insure that the classes provided here are correct.  The tests exercise
the Google Apps APIs, but do not make any calls to Google, so it is
not necessary to set up any authentication credentials just to run the tests.

1. Clone this repository
1. Run `composer install`
1. Run `./vendor/bin/phpunit tests`

All of the tests are also run by [Travis CI](https://travis-ci.org/westkingdom/hierarchical-group-email) on every commit.

## Basic Example

If you follow the instructions in the following sections, code similar to
the basic overview shown below should work.
```
use Westkingdom\HierarchicalGroupEmail\GroupsManager;

$groupsManager = GroupsManager::createForDomain('My application', 'mydomain.org', $currentState);
$currentState = $groupsManager->update($newState);
```
Even if you use this simple form, you need to understand how this API
searches for and uses your authentication data, and how to manage the state
of your data.  See below for more details on how this works.

### Composer Instructions

The best way to install this library in your application is to use
Composer.  Simply add the following line to your composer.json file's
`require` section:
```
{
  "require": {
    "westkingdom/hierarchical-group-email": "~1"
  }
}
```
To use Composer with popular content management systems, please see
the following resources:

- Drupal: [Composer Generate](https://www.drupal.org/project/composer_generate)
- Joomla: [Getting Started with Composer and Joomla!](http://magazine.joomla.org/issues/issue-aug-2013/item/1450-getting-started-with-composer-and-joomla)
- Wordpress: [Using Composer with WordPress](https://roots.io/using-composer-with-wordpress/)

Of course, it is possible to use this library without composer; you just
need to be responsible for setting up the autoloader, or including the
class files yourself.  However, using Composer is strongly recommended.

### Configuring Your Authentication Information

Follow the [authorization information setup instructions](http://docs.westkingdom.org/en/latest/google-api/) on the 
[documentation website](http://docs.westkingdom.org).

### Prepare Your Data

This library expects you to accumulate all of the information about all
of your groups, and their memberships in a nested heirarchical array.

The structure is shown below in yaml, but you may store it in whatever
format is most convenient for your application.
```
GROUPNAME:
  lists:
    OFFICENAME:
      members:
        - user1@domain1.org
        - user2@domain2.org
      properties:
        group-name: 'Full name of Office'
  subgroups:
    - subgroup1
    - subgroup2
    - subgroup3
```
Just repeat this structure for as many groups as you have.  If your groups
are heirarchical in nature, just name the "child" groups in each group's
"subgroup" section.  The names listed should exaclty match the "GROUPNAME"
used as the key for the group's data.

Note also that it is the responsibility of the caller to keep track of
the current state and the new state.  The group manager will send updates
for just the changes that occure in the new state compared to the old state.
If you do not provide the current state, then groups will never be deleted,
and group members will never be removed.

Future: The group manager could provide an "export" function to build the
current state of the groups by calling the Google API.

## Expanded Example

If you would like more control over what happens in an update, you can
construct the internal classes yourself and modify them before making your
GroupsManager.

```
use Westkingdom\HierarchicalGroupEmail\ServiceAccountAuthenticator;
use Westkingdom\HierarchicalGroupEmail\StandardGroupPolicy;
use Westkingdom\HierarchicalGroupEmail\GoogleAppsGroupsController;
use Westkingdom\HierarchicalGroupEmail\GroupsManager;

$authenticator = ServiceAccountAuthenticator("My application");
$client = $authenticator->authenticate();
$policy = new StandardGroupPolicy('mydomain.org', $properties);
$controller = new GoogleAppsGroupsController($client);
$groupManager = new GroupsManager($controller, $policy, $currentState);
$currentState = $groupManager->update($newState);
```

Note: If you also want to control the behavior of the batch operations,
you can provide a batch object to the GoogleAppsGroupsController constructor.
See below for details.

### Create a Service Authenticator

A service authenticator will help your application load its authentication
credential information from well-known files, so they do not need to be
hard-coded in the application source code.

`$authenticator = ServiceAccountAuthenticator("My application", $searchpath);`

"My application" is the name of your application; this will be passed
to any Google_Client created by the authenticator.

`$searchpath` is an array of paths to search for authentication files.
Relative paths are resolved relative to the current user's home directory.
The default searchpath is:

- .google-api
- /etc/google-api

### Authenticate

`$client = $authenticator->authenticate($serviceAccount, $scopes, $serviceToken);`

### Create a Standard Group Policy

`$policy = new StandardGroupPolicy('mydomain.org', $properties);`

'mydomain.org' is the base domain for your Google Apps account.
`$properties` contain default values for properties used by the policy.
See below for a list of the different properties that can be set
to customize the operation of the library.

### Create a Google Apps Group Controller

The Google Apps Group Controller is the object that actually talks
to the Google API.  Batch mode is always used; you can manage the
batch object yourself, as shown below:

```
$batch = new \Google_Http_Batch($client);
$controller = new GoogleAppsGroupsController($client, $policy, $batch);
$groupManager = new GroupsManager($controller, $currentState);
...
// When finished:
$groupManager->execute();
```

If you do not want to manage the batch object, just leave off those
lines, and the contorller will create and execute batches as needed.
This is the preferred method of operation; for example, the controller
will try to arrange to run the batch commands to create new groups before
it runs any of the commands to add members to a group; this makes it
more likely that the batch commands will complete successfuly.

### Create a Groups Object and Update It

The Groups object is responsible for evaluating how the new state
differs from the current state.  It then instructs the controller to make
whatever changes are necessary to update the current state to match
the new state.

```
$groupManager = new Groups($controller, $currentState);
$groupManager->update($newState);
```
Changes are always made in batch mode.  Batch mode can be handled for
you, or you can control it yourself, as shown in the previous section.

### Debugging, Logging or Prompting

If you'd like to know what the GroupManager is going to do before it
does it, you can use a BatchWrapper object.

```
use Westkingdom\HierarchicalGroupEmail\BatchWrapper;

$client->setUseBatch(true);
$batch = new \Google_Http_Batch($client);
$batchWrapper = new BatchWrapper($batch);
$controller = new GoogleAppsGroupsController($client, $policy, $batch);
...
// To log or prompt or whatever:
$operationList = $batchWrapper->getSimplifiedRequests();

// When finished:
$batchWrapper->execute();
```
If you are only reporting / debugging, it is not necessary to create the
Google_Http_Batch at all; you can just use the BatchWrapper by itself.

## Property Directory

Properties can contain replacements, which come in two forms.

- $(var): Substitutes the all-lowercase form of the variable
- ${var}: Substitutes the variable name 

Property              | Default Value                  | Description
--------------------- | ------------------------------ | -----------
subdomains            | n/a                            | Comma-separated list of GROUPNAMES that are also subdomains.
group-name            | ${branch} ${office}            | Template used to name groups.
group-email           | $(branch)-$(office)@$(domain)  | Template used to generate group's standard email address.
top-level-group       | Domain without .tld            | Identifies the group at the top of the heirarchy.
top-level-group-email | $(office)@$(domain)            | Short form alternate email for the top-level group.
subdomain-group-email | $(office)@$(branch).$(domain)  | Short form alternate email for all groups with a subdomain.
aggragated-groups     | n/a                            | Data used to generate custom aggregated groups.
aggragate-all-name    | All ${office-plural}           | Template for the name of the "all officers" aggregated group.
aggragate-all-key     | all-$(office-plural)           | Template used to generate the key for the "all officers" aggregated groups.
aggragate-all-email   | all-$(office-plural)@$(domain) | Template used to generate the standard email address for the "all officers" aggregated groups.
aggragate-all-alternate-email | $(office-plural)@$(domain) | Short form alternate email for the "all officers" aggregated groups. Ignored if it does not generate a unique pattern.
aggragate-branch-officers-name | ${branch} Officers    | Template used to generate the name of the "branch group officers" aggregated groups.
aggragate-branch-officers-key | $(branch)-officers     | Template used to generate the key for the "branch group officers" aggregated groups.
aggragate-branch-officers-email | $(branch)-officers@$(domain) | Template used to generate the standard email address for the "branch group officers" aggregated groups.
subdomain-aggragate-branch-officers-email | officers@$(branch).$(domain) | Short form alternate email for the "branch group officers" aggregated groups. Ignored if it does not generate a unique pattern.
tld-aggragate-branch-officers-email | officers@$(domain) | Short form alternate email for the "branch group officers" for the top-level group.
aggragate-all-subgroup-name | All ${subgroup} ${office-plural} | Template for the "all officers" aggregated group for branch groups below the top-level group.
aggragate-all-subgroup-key | $(subgroup)-all-$(office-plural) | Key for same.
aggragate-all-subgroup-email | $(subgroup)-all-$(office-plural)@$(domain) | Template used to generate the standard email address for the "all officers" aggregated group for branches that are subdomains.
subdomain-aggragate-all-subgroup-email | all-$(office-plural)@$(subgroup).$(domain) | Template used to generate the standard email address for the "all officers" aggregated group for branches that are subdomains.
subdomain-aggragate-all-subgroup-alternate-email | $(office-plural)@$(subgroup).$(domain) | Shortened form for same.
primary-office | n/a | The office that should by default be the primary email contact for the group (e.g. the president). The primary officer is given an alternate email address named after the group.
primary-office-alternate-email-principal-group | $(branch)@$(domain) | Template used to generate the alternate email address for the primary officer.
primary-office-alternate-email-branch-group | $(branch)@$(parent).$(domain) | Short form for same, when the parent group is a subdomain.
member-email | $(branch)-$(member)@$(domain) |
aggregate-group-email | $(aggregate)@$(branch).$(domain) |

