/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpError.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpisak-l <bpisak-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/19 10:56:37 by bpisak-l          #+#    #+#             */
/*   Updated: 2024/11/13 15:50:33 by bpisak-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpError.hpp"

HttpError::HttpError() : _message("Undefined Error"), _code(200) {};

HttpError::HttpError(const std::string &msg, const int code, const std::map<std::string, std::string> &extraHeaderFields) : _message(msg), _code(code), _extraHeaderFields(extraHeaderFields) {};

HttpError::~HttpError() noexcept {};

HttpError::HttpError(HttpError const &ex) { *this = ex; };

HttpError &HttpError::operator=(const HttpError &ex)
{
	if (this != &ex)
	{
		_message = ex.getMessage();
		_code = ex.getCode();
		_extraHeaderFields = ex._extraHeaderFields;
	}
	return *this;
};

std::string const &HttpError::getMessage() const { return _message; };

const std::map<std::string, std::string> &HttpError::getExtraFields() const
{
	return _extraHeaderFields;
};

int HttpError::getCode() const { return _code; };

const char *HttpError::what() const noexcept
{
	return _message.c_str();
}

void HttpError::setField(std::string key, std::string value)
{
	_extraHeaderFields[key] = value;
}