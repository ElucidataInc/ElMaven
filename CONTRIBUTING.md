# Contributing to ElMaven

Following are some guidelines for contributing to El-Maven. You may propose changes to the document in a pull request. 

# For Users

## Filing Issues
Bug reports/feature requests are welcome! Please keep certain things in mind before filing an issue:
* Do a search [here](https://github.com/ElucidataInc/ElMaven/issues) to check if it has already been reported. In case you find a closed issue that matches your search:
  * Install the latest version
  * If the problem persists, request to reopen it via comments
* Create a new issue for every bug, crash, feature request or enhancement
* Make sure to mention the version/branch you are using
* For bugs/crashes, please mention step-by-step procedure to reproduce the issue
* Add screenshots/attach files when possible. They help us understand the issues better
* Proper labels should be assigned to the issue

You can file a new issue [here](https://github.com/ElucidataInc/ElMaven/issues/new). 

# For Developers

## Getting Started
* Make sure you have a [GitHub account](https://github.com/signup/free)
* Fork the repository on GitHub
* Set up your system to run El-Maven. You can find the instructions [here](https://elucidatainc.github.io/ElMaven/)
* Go through the code documentation and commit guidelines
* Assign yourself to an open issue or file your own
* Pull a new branch from develop. Branch name should reflect the issue addressed in it
* Get started!



## Commit Guidelines
* Start the subject line as "Ftr:", "Fix:", "Doc:", "Test:","Ref:" etc.
  Ftr-for adding feature
  Fix-for fixing bug
  Doc-for documentation
  Test- for testing
  Ref-for code refactoring
* At the end of subject line, tag issue no
* Separate subject from body with a blank line
* Limit the subject line to 50 characters- it's a rule of thumb, follow it strictly
  as long as possible otherwise long subject line will get truncated.
* Capitalize the subject line
* Do not end the subject line with a period
* Use the imperative mood in the subject line i.e. as you are giving command 
  or instrcution
  examples-all these commit guidelines have imperative mood
* Keep the length of line in body at 72 character
* Use the body to explain what and why vs how in concise manner i.e. no story telling 
  and no inferential statements
* Tag the issue number as last line of body, for example- Issue: #324 with 
  first letter capital and at end a colon and tag recommended issue to look over after this line if it's required
 
  For example:

  		Doc:Giving a good commit message subject line #324

  		This commit message example is a good example of its own. Its  subject
  		line has 50 or less than 50 characters and body lines have 72 or  less
  		than 72 characters.This paragraph  of this message  is explaining what
  		changes I'm making, next paragraph is going to  explain why I'm making
  		this change and next of next one is going to  explain  how I'm  making
  		this change.

		Proper commit message is neccessary to understand the changes someone is
		making and  impact  of these changes  to the codebase. It  helps fellow 
		developers to understand the codebase and to save their time for faster
		delivery of product. It also evaluates colaboration skill of a developer.

		* First I have provided instructions to write good commit message.
		* Next I have given an example.
		* In this section I'm using bullet points,it could be "-" or "*" .
		* Bullet points are not neccessary. It can also be simple paragraph.
		* Last line have tag for issue this commit is going so resolve.
		* There is no other issue related to this issue, that's why there is
		  no "See also: #IssueNo, #IssueNo ..." line after last line.

		Issue: #324





## Documentation guidelines
We make use of Doxygen for generating the code documentation. You can read more about Doxygen [here](http://www.stack.nl/~dimitri/doxygen/index.html).

We follow JavaDoc style comment blocks  to document classes and functions and Qt style comment blocks to document class members.
Read about comment blocks [here](http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html#cppblock)

### How to document
In this short guide we will see how to document functions,classes, members of class.
* While documenting classes/functions make sure to place the comment blocks before the declaration.
* While documenting members of class, make sure to place the comment block after the declaration.
* The keywords to be used are explained below
* Refer to these [examples](http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html#docexamples)  to better understand how to use the comment blocks

1. Function documentation

    * @brief - A short description about what the function does. Do not use more than 2 lines
    * @details - Use this to explain what function does in detail.This can be a bit long depending on how well  we need to explain the      funtion
    * @param - If necessary explain the parameters. Try to keep it short
    * @return - If necessary explain what the function returns. Try to keep it short
    * @see - Use this if the function is related to any other function/member. To create links just mention the name of the fuction/member.
             If the function to be linked is defined in some other class, use className::functionName to create link and make sure the other function is also documented otherwise
             the link won't be created


2. class documentation
    * @brief - A short description about what the class does.
    * @details - Use this to explain what's the role of the class, who handles the creation/deletion of the class etc
    * @author - If necessary, mention the author/mail address

3. class/struct/enum member documentation- Use the following comment style for members
   * int variable; /**<  describe what this variable does */

## Creating Pull Requests
Make your PR review process faster by maintaining this checklist before creating a pull request:
- The code builds successfully on your machine
- No test case is failing
- Unit tests have been added/modified to cover your changes
- The PR is linked to some open issue
- One PR is not addressing more than one issue
- There is a separate PR for code refactoring
- Code documentation guidelines have been followed
- Commit guidelines have been followed
- Title and description of the PR provide an overview of all changes made in the branch

You can open a new pull request [here](https://github.com/ElucidataInc/ElMaven/compare). 
