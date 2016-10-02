---
layout: post
permalink: /jekyll_config
---
Fire Jekyll Configuration
=

Fire uses Jekyll for publishing documentation on GitHub Pages and it uses the
[Gravity theme](1). There are several important things to keep in
mind for this configuration. The Gemfile should contain
```ruby
# If you have OpenSSL installed, we recommend updating
# the following line to use "https"
source 'http://rubygems.org'

# Configure the gem for working with GitHub Pages
gem 'github-pages', group: :jekyll_plugins

# Configure the asciidoc support in Jekyll
group :jekyll_plugins do
  gem 'jekyll-asciidoc'
end
```

I ran into two major problems with GitHub that took hours to fix and then only
after contacting GitHub directly.
- The site.url (or just "url") and site.baseurl variables in _config.yml must be set to 
"http://<username>.github.io/<project>" for a project level deployment as in
```
url = "http://jayjaybillings.github.io"
baseurl = "/fire"
```
- The only valid places to put github.io files are in the master branch, master/docs
or in gh-pages. I used gh-pages/doc and it would not work until I merged
into master/docs.

The Gravity theme works as expected when deployed using
```shell
bundle exec jekyll serve
```

but deployment to GitHub fails because the style.scss file has references to
empty scss files in _ssas directory starting at line 47. When these imports
are removed or commented out, GitHub will build the site. 

Thanks to @AlexPawlowski_ for encouraging me to not give up and sharing some expertise.

[1]: http://hemangsk.github.io/Gravity/
