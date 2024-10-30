# Contributing to laser

Thanks for taking the time to contribute! :heart:

All types of contributions are encouraged and valued. 
See the [Table of Contents](#table-of-contents) for different ways to help and 
details about how this project handles them. Please make sure to read the 
relevant section before making your contribution.

## Table of Contents

- [I Have a Question](#i-have-a-question)
- [I Want To Contribute](#i-want-to-contribute)
- [Reporting Bugs](#reporting-bugs)
- [Suggesting Enhancements](#suggesting-enhancements)
- [Your First Code Contribution](#your-first-code-contribution)
- [Improving The Documentation](#improving-the-documentation)
- [Styleguides](#styleguides)
- [Commit Messages](#commit-messages)
- [Join The Project Team](#join-the-project-team)

## I Have a Question

Before you ask a question, it is best to search for existing 
[Issues](https://github.com/jmattaa/laser/issues) that might help you. 
In case you have found a suitable issue and still need clarification, 
you can write your question in this issue.

If you then still feel the need to ask a 
question and need clarification, we recommend the following:

- Open an [Issue](https://github.com/jmattaa/laser/issues/new).
- Provide as much context as you can about what you're running into.

We will then take care of the issue as soon as possible.

## I Want To Contribute

### Reporting Bugs

#### Before Submitting a Bug Report

A good bug report shouldn't leave others needing to chase you up for more 
information. Therefore, we ask you to investigate carefully, collect 
information and describe the issue in detail in your report.
Please complete the following steps in advance to help us fix any potential bug 
as fast as possible.

### Suggesting Enhancements

This section guides you through submitting an enhancement suggestion for laser, 
**including completely new features and minor improvements to existing 
functionality**. 

#### Before Submitting an Enhancement

- Make sure that you are using the latest version.
- Perform a [search](https://github.com/jmattaa/laser/issues) to see 
if the enhancement has already been suggested. If it has, add a comment to the 
existing issue instead of opening a new one.
- Find out whether your idea fits with the scope and aims of the project. It's 
up to you to make a strong case to convince the project's developers of the 
merits of this feature. Keep in mind that we want features that will be useful 
to the majority of our users and not just a small subset. 

#### How Do I Submit a Good Enhancement Suggestion?

Enhancement suggestions are tracked as 
[GitHub issues](https://github.com/jmattaa/laser/issues).

- Use a **clear and descriptive title** for the issue to identify the suggestion.
- Provide a **step-by-step description of the suggested enhancement** in as 
many details as possible.
- **Describe the current behavior** and **explain which behavior you expected 
to see instead** and why. 
- **Explain why this enhancement would be useful** to most laser users. You may 
also want to point out the other projects that solved it better and which could 
serve as inspiration.

### Your First Code Contribution

#### Development Environment Setup

1. Ensure you have the following prerequisites:
   - GCC or Clang compiler
   - Make build system
   - Git

2. Fork the repository

3. Clone the repository:
   ```bash
   git clone https://github.com/[yourusername]/laser.git
   cd laser
   ```

4. Build the project:
   ```bash
   make
   ```

#### Code formatting 

The project uses `.clang-format` for consistent code styling.

1. Use spaces instead of tabs (4 spaces per indent)
2. Enable clang-format integration if available
3. Set maximum line length to 80 characters

### Improving The Documentation

Documentation improvements are always welcome! Here are some guidelines:

1. **Code Documentation**
   - Add comments for complex algorithms or non-obvious code
   - Keep function documentation up-to-date
   - Document public APIs thoroughly

2. **README Updates**
   - Keep installation instructions current
   - Document new features
   - Update usage examples

3. **Contributing Guide**
   - Help improve this guide
   - Add missing sections
   - Clarify existing instructions

4. **Documentation Style**
   - Use clear, concise language
   - Include examples where helpful
   - Keep formatting consistent

## Styleguides
### Commit Messages

Follow these guidelines for commit messages:

1. **Format**
   ```
   <type>: <subject>

   [optional body]
   [optional footer]
   ```

2. **Types**
   - `feat`: New feature
   - `fix`: Bug fix
   - `docs`: Documentation changes
   - `style`: Code style changes (formatting, etc)
   - `refactor`: Code refactoring
   - `test`: Adding or updating tests
   - `chore`: Maintenance tasks

3. **Guidelines**
   - Use present tense ("add feature" not "added feature") 
   (so my english teacher is happy)
   - First line should be 50 characters or less
   - Be descriptive but concise

4. **Examples**
   ```
   feat: add git status integration
   
   fix: correct directory sorting algorithm
   
   docs: update installation instructions
   ```
